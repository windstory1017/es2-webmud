#!/usr/bin/env bash
# 開 PR 自動檢查：跑 tests/ 裡的全部測試（docs/CI_AUTO_CHECK.md）。
# .py 用 pytest 一次真的執行每一項 test_ 函式（以前是 python3 逐檔直接執行，
# 沒有寫執行入口的檔案只會載入、不會驗證，永遠「通過」）；.mjs 用 node 逐一執行。
# 需要先安裝 pytest（pip install pytest），並在 server/ 執行過 npm ci（.mjs 測試會用到）。
# 任何一個失敗，最後就以非 0 結束。
set -u
cd "$(dirname "$0")/.."

FAILED=()

# 有些測試會重新產生這幾個檔案（不是修改內容）；跑完還原，避免被誤提交。
restore_generated() {
    git checkout -- catalog/v13_fixture/manifest.json catalog/generated/content_workbench.json reports 2>/dev/null || true
}
trap restore_generated EXIT

echo "== pytest：tests/*.py =="
start=$SECONDS
if out=$(timeout 900 python3 -m pytest -q -p no:cacheprovider --tb=short tests/ 2>&1); then
    echo "$out" | tail -5
    echo "pytest 通過（$((SECONDS - start)) 秒）"
else
    rc=$?
    [ "$rc" = 124 ] && why="超過 15 分鐘" || why="結束代碼 $rc"
    echo "::error::pytest 測試失敗（$why）"
    echo "$out" | tail -120
    FAILED+=("pytest tests/")
fi

echo
echo "== node：tests/*.mjs =="
MJS_PASSED=0
for t in tests/*.mjs; do
    start=$SECONDS
    if out=$(timeout 300 node "$t" 2>&1); then
        MJS_PASSED=$((MJS_PASSED + 1))
        echo "通過  $t（$((SECONDS - start)) 秒）"
    else
        rc=$?
        FAILED+=("$t")
        [ "$rc" = 124 ] && why="超過 5 分鐘" || why="結束代碼 $rc"
        echo "::error file=$t::測試失敗（$why）"
        echo "失敗  $t（$why）"
        echo "$out" | tail -40 | sed 's/^/      /'
    fi
done

echo
echo "node 測試通過 $MJS_PASSED 個，失敗 $(( ${#FAILED[@]} - $(printf '%s\n' "${FAILED[@]:-}" | grep -c '^pytest') )) 個"
if [ ${#FAILED[@]} -gt 0 ]; then
    printf '  失敗：%s\n' "${FAILED[@]}"
    exit 1
fi
