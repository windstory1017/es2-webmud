# 開 PR 時自動檢查（使用說明）

對應 `docs/IMPROVEMENT_ROADMAP.md` 第 1 項；規劃與確認紀錄見 `docs/CI_AUTO_CHECK_PLAN.md`。

## 做了什麼

每次在 `wolfer168/es2-webmud` 開 PR（或 PR 有新的修改），以及 `main` 有新的合併時，
GitHub 會在一台用完就丟的臨時機器上依序做這幾件事：

| 步驟 | 內容 | 會抓到的問題 |
|---|---|---|
| 1 | 編譯遊戲伺服器（跟 `update.sh` 同樣的指令） | `source/upstream/neolith/` 改壞 |
| 2～3 | 開機；新建角色 `citest`（人類、男性）→ `score` → `quit`；再用同一個角色登入 → `look` → `quit` | `login.c`、`user.c`、`logind.c`、種族、出生房間等讓「全服登不進去」的錯誤；開機與登入過程中任何程式載入錯誤 |
| 4 | 啟動網頁橋接程式，確認健康檢查網址有回應 | `server/` 改壞 |
| 5 | 跑 `tests/` 裡全部測試（目前 121 個） | 各項既有功能被改壞 |

整套大約 3 分鐘。臨時機器跟 Oracle 正式主機無關，不會碰到玩家資料。

相關檔案：

| 檔案 | 用途 |
|---|---|
| `.github/workflows/pr-check.yml` | GitHub 自動檢查的設定 |
| `tools/ci_boot_login.py` | 開機與模擬登入（步驟 2～3） |
| `tools/ci_run_tests.sh` | 逐一跑測試（步驟 5） |

## 紅色叉叉怎麼看

1. 在 PR 頁面最下方的檢查區，點 `pr-check` 旁邊的「Details」。
2. 左邊會列出每個步驟，打叉的那一步就是出問題的地方，點開看紀錄：
   - **編譯**：紀錄最後幾行會寫哪個 `.c` 檔第幾行錯。
   - **開機與登入**：會寫卡在哪一步（例如「選性別：遊戲中斷了連線」），附上最後收到的遊戲畫面，
     以及紀錄檔裡的錯誤（例如 `log/log：obj/user.c:322: syntax error`）。
   - **跑全部測試**：失敗的測試會標成紅色，並附上該測試最後 40 行輸出；最下面有失敗清單。
3. 修好後推上同一個分支，檢查會自動重跑。

自己電腦或主機上也可以跑同樣的檢查（要在剛 clone 的乾淨資料夾裡跑，因為會建立測試角色）：

```
python3 tools/ci_boot_login.py --neolith source/upstream/neolith/out/build/linux/src/RelWithDebInfo/neolith
(cd server && npm ci --omit=dev) && tools/ci_run_tests.sh
```

## 「有錯不准合併」要 wolfer168 在 GitHub 打開

自動檢查本身只會亮紅燈；要真的擋住合併，需要 repo 擁有者（wolfer168）設定一次：

1. 打開 https://github.com/wolfer168/es2-webmud/settings/branches
2. 按「Add branch ruleset」（或「Add classic branch protection rule」）。
3. 目標分支選 `main`（classic 規則的 Branch name pattern 填 `main`）。
4. 勾選「Require status checks to pass」（classic：「Require status checks to pass before merging」），
   在搜尋框輸入 `pr-check` 並加入。
   （`pr-check` 至少要在 PR 上跑過一次，搜尋框才找得到。）
5. 儲存。

之後 `pr-check` 沒變綠燈前，PR 的合併按鈕會是灰的。

## 之後可以加的

- 把所有房間、NPC、武功檔都載入一次，抓登入碰不到的檔案的語法錯誤（2026-10-24 確認先不做）。
- 路線圖第 14、15 項（遊戲文字、說明檔格式的自動檢查）可以掛在同一套流程。
