# 更新是否需要重開（判斷表）

使用者指示（2026-10-24）：每次「開 PR」的最後，都要標明這次更新是否需要重開，並附上主機要執行的指令。
這裡的「重開」指重開遊戲服務（`update.sh` 會做的事，玩家斷線幾秒到十幾秒），不是把 Oracle 主機整台關機重開；
整台主機只有在系統核心（kernel）更新時才需要重開。

## 1. 需要重開遊戲

| 改到的地方 | 位置 |
|---|---|
| 遊戲伺服器本身 | `source/upstream/neolith/` |
| 伺服器設定 | `source/upstream/neolith.conf` |
| 核心主程式 | `mudlib/adm/obj/master.c` |
| 共用函式 | `mudlib/adm/obj/simul_efun.c`、`mudlib/adm/simul_efun/` |
| 共用定義檔 | `mudlib/include/*.h` |
| 一開機就載入的系統程式 | `mudlib/adm/etc/preload` 列出的檔案（logind、securityd、cmd_d、realipd 等） |
| 角色與登入物件 | `mudlib/obj/user.c`、`mudlib/obj/login.c` |
| 底層共用功能 | `mudlib/feature/`、`mudlib/std/`（技能系統、戰鬥、武功共用引擎等） |
| 系統常駐程式 | `mudlib/adm/daemons/` |
| 網頁橋接程式 | `server/`（只需重開網頁服務 `es2-web`） |

指令：
```
bash ~/es2-webmud/deploy/oracle/update.sh
```
只改 `server/` 時也可以只重開網頁服務：拉程式後 `sudo systemctl restart es2-web`。

## 2. 不需要重開遊戲

| 改到的地方 | 位置 | 生效方式 |
|---|---|---|
| 房間 | `mudlib/d/`、`mudlib/custom/*/room/` | 巫師 `update <檔名>`（站在房間裡可用 `update here`） |
| NPC | `mudlib/d/*/npc/`、`mudlib/custom/legacy_npc/` 等 | `update <檔名>`；地圖上已有的要等房間重置或清掉重生 |
| 物品、裝備 | `mudlib/obj/`（`user.c`、`login.c` 除外）、`mudlib/d/*/obj/`、`mudlib/custom/armor/` 等 | `update <檔名>`；玩家身上已有的是舊版 |
| 武功、法術 | `mudlib/daemon/skill/` | `update <技能檔>` |
| 玩家指令 | `mudlib/cmds/` | `update <指令檔>` |
| 說明檔 | `mudlib/docs/help/` | 直接生效 |
| 文件、設計文件 | `docs/` | 不影響遊戲 |

指令（只拉程式、不重開）：
```
cd ~/es2-webmud && git fetch origin main && git checkout -B main origin/main
```
之後巫師在遊戲裡依清單 `update` 各檔案。

## 3. 網頁部分

| 改到的地方 | 生效方式 |
|---|---|
| `web/` | 拉程式後玩家按 F5 重新整理 |
| 玩家版圖鑑（`tools/build_player_codex.py`、`tools/codex_player_template.html`、`docs/catalog/玩家圖鑑_*.md`，或任何遊戲內容變動） | `python3 ~/es2-webmud/tools/build_player_codex.py`，玩家重新整理圖鑑頁 |
| `server/` | 重開網頁服務：`sudo systemctl restart es2-web` |

## 免重開模式（`update.sh hot`）

不確定這次要不要重開時，先跑：
```
bash ~/es2-webmud/deploy/oracle/update.sh hot
```
它會備份玩家資料、拉最新程式、重新產生玩家版圖鑑，**不重開、不編譯**，然後依上面的判斷表把改到的檔案分三類列出：
需要重開的（改跑一般的 `update.sh`）、可在遊戲裡 `update` 的（附可直接貼上的指令）、網頁檔案（玩家按 F5；改到 `server/` 要重開 `es2-web`）。
若這次含 Neolith 原始碼或 `server/package*` 的變動，hot 會留下記號，之後跑一般的 `update.sh` 會補編譯。
不加 `hot` 時，`update.sh` 的行為不變。

## 快速查看這次改了哪些檔案

```
cd ~/es2-webmud && git fetch origin main && git diff --name-only HEAD origin/main
```
