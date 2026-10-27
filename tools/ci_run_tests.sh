#!/usr/bin/env bash
# 開 PR 自動檢查：逐一跑 tests/ 裡的測試（docs/CI_AUTO_CHECK.md）。
# .py 用 python3、.mjs 用 node，每個最多 5 分鐘；任何一個失敗，最後就以非 0 結束。
# 需要先在 server/ 執行過 npm ci（有些測試會用到網頁橋接程式的套件）。
set -u
cd "$(dirname "$0")/.."

PASSED=0
FAILED=()
for t in tests/*.py tests/*.mjs; do
    case "$t" in
        *.py)  cmd=(python3 "$t") ;;
        *.mjs) cmd=(node "$t") ;;
    esac
    start=$SECONDS
    if out=$(timeout 300 "${cmd[@]}" 2>&1); then
        PASSED=$((PASSED + 1))
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
echo "共 $((PASSED + ${#FAILED[@]})) 個測試：通過 $PASSED 個，失敗 ${#FAILED[@]} 個"
if [ ${#FAILED[@]} -gt 0 ]; then
    printf '  失敗：%s\n' "${FAILED[@]}"
    exit 1
fi
