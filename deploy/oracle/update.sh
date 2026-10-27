#!/bin/bash
# 雲端主機更新到最新版本：備份玩家資料 → 拉最新程式 → 需要時重新編譯 → 清快取 → 重開。
# 用法：bash ~/es2-webmud/deploy/oracle/update.sh        （一般更新，會重開遊戲服務）
#       bash ~/es2-webmud/deploy/oracle/update.sh hot    （免重開模式：只備份、拉程式、重產圖鑑，
#                                                         並依 docs/UPDATE_RESTART_GUIDE.md 列出該做的事，不重開）
set -euo pipefail

HOT=0
case "${1:-}" in
    "") ;;
    hot) HOT=1 ;;
    *) echo "用法：bash update.sh [hot]"; exit 1 ;;
esac

BRANCH="${BRANCH:-main}"
APP_DIR="${APP_DIR:-$HOME/es2-webmud}"
MUD="$APP_DIR/source/upstream/mudlib"
BACKUP_DIR="${BACKUP_DIR:-$HOME/es2-backups}"
# hot 模式拉了程式卻沒編譯時留下記號，之後跑一般的 update.sh 才會補編譯。
PENDING="$APP_DIR/.git/es2-hot-pending-build"

# 免重開模式的報告：把這次改到的檔案依 docs/UPDATE_RESTART_GUIDE.md 分成三類。
hot_report() {
    local RESTART="" WIZ="" WEB=0 CODEX=0 SERVER=0 f p
    while IFS= read -r f; do
        [ -n "$f" ] || continue
        case "$f" in
            source/upstream/neolith/*|source/upstream/neolith.conf) RESTART+="$f"$'\n' ;;
            source/upstream/mudlib/adm/*|source/upstream/mudlib/include/*|source/upstream/mudlib/feature/*|source/upstream/mudlib/std/*) RESTART+="$f"$'\n' ;;
            source/upstream/mudlib/obj/user.c|source/upstream/mudlib/obj/login.c) RESTART+="$f"$'\n' ;;
            source/upstream/mudlib/daemon/misc/npc_power.c|source/upstream/mudlib/daemon/misc/npc_skill.c) RESTART+="$f"$'\n' ;;
            server/*) RESTART+="$f"$'\n'; SERVER=1 ;;
            source/upstream/mudlib/*.c) p="${f#source/upstream/mudlib}"; WIZ+="update $p"$'\n' ;;
            web/*) WEB=1 ;;
            tools/build_player_codex.py|tools/codex_player_template.html|docs/catalog/玩家圖鑑_*) CODEX=1 ;;
        esac
    done <<< "$1"

    echo
    echo "==== 這次更新要做的事（免重開模式）===="
    if [ -z "$RESTART$WIZ" ] && [ "$WEB$CODEX$SERVER" = "000" ]; then
        echo "沒有需要處理的變動。"
        return
    fi
    if [ -n "$RESTART" ]; then
        echo
        echo "【需要重開遊戲】以下檔案要重開才會生效，hot 模式不會重開："
        echo "$RESTART" | sed '/^$/d; s/^/  /'
        echo "→ 請選好時間改跑一般更新：bash ~/es2-webmud/deploy/oracle/update.sh"
    fi
    if [ -n "$WIZ" ]; then
        echo
        echo "【可在遊戲裡 update】巫師登入後貼上這些指令（房間、NPC、物品記得依需要重置或清掉重生）："
        echo "$WIZ" | sed '/^$/d; s/^/  /'
    fi
    if [ "$WEB" = 1 ] || [ "$CODEX" = 1 ] || [ "$SERVER" = 1 ]; then
        echo
        echo "【網頁檔案】"
        if [ "$WEB$CODEX" != "00" ]; then
            echo "  已更新，請玩家按 F5 重新整理（圖鑑已重新產生）。"
        fi
        if [ "$SERVER" = 1 ]; then
            echo "  改到 server/：網頁橋接程式要重開才生效：sudo systemctl restart es2-web（玩家會短暫斷線）"
        fi
    fi
}

echo "==== 1/4 備份玩家資料到 $BACKUP_DIR ===="
mkdir -p "$BACKUP_DIR"
STAMP=$(date +%Y%m%d-%H%M%S)
tar -czf "$BACKUP_DIR/data-$STAMP.tar.gz" -C "$MUD" data
ls -1t "$BACKUP_DIR"/data-*.tar.gz | tail -n +15 | xargs -r rm -f   # 只留最近 14 份
echo "已備份：data-$STAMP.tar.gz"

echo "==== 2/4 拉最新程式（分支 $BRANCH）===="
OLD=$(git -C "$APP_DIR" rev-parse HEAD)
git -C "$APP_DIR" fetch origin "$BRANCH"
git -C "$APP_DIR" checkout -B "$BRANCH" "origin/$BRANCH"
NEW=$(git -C "$APP_DIR" rev-parse HEAD)

if [ "$HOT" = 1 ]; then
    CHANGED=$(git -C "$APP_DIR" diff --name-only "$OLD" "$NEW")
    # 有要編譯的東西就留記號，等一般的 update.sh 來補。
    if echo "$CHANGED" | grep -qE '^(source/upstream/neolith/|server/package)'; then
        touch "$PENDING"
    fi
    echo "==== 3/4 免重開模式：重新產生玩家版圖鑑 ===="
    python3 "$APP_DIR/tools/build_player_codex.py" || echo "（圖鑑產生失敗，沿用舊的圖鑑）"
    mkdir -p "$MUD/log/static"
    hot_report "$CHANGED"
    exit 0
fi

echo "==== 3/4 Neolith 原始碼有變動才重新編譯 ===="
if [ -f "$PENDING" ] || { [ "$OLD" != "$NEW" ] && git -C "$APP_DIR" diff --name-only "$OLD" "$NEW" | grep -q '^source/upstream/neolith/'; }; then
    # 記憶體不足 2GB 時一次只編譯一個檔案，避免記憶體用光而中斷。
    MEM_MB=$(awk '/MemTotal/ {print int($2/1024)}' /proc/meminfo)
    [ "$MEM_MB" -lt 2000 ] && export CMAKE_BUILD_PARALLEL_LEVEL="${CMAKE_BUILD_PARALLEL_LEVEL:-1}"
    cd "$APP_DIR/source/upstream/neolith"
    cmake --preset linux
    cmake --build --preset pr-linux --target neolith
else
    echo "沒有變動，略過。"
fi
if [ -f "$PENDING" ] || git -C "$APP_DIR" diff --name-only "$OLD" "$NEW" | grep -q '^server/package'; then
    (cd "$APP_DIR/server" && npm ci --omit=dev)
fi
rm -f "$PENDING"

# 玩家版天朝圖鑑（web/codex.html）依目前的遊戲內容重新產生；失敗也不影響更新。
python3 "$APP_DIR/tools/build_player_codex.py" || echo "（圖鑑產生失敗，沿用舊的圖鑑）"
mkdir -p "$MUD/log/static"   # 自殺、複製物品等事件紀錄要寫在這裡（舊主機可能沒有這個資料夾）
# 自動安全更新（unattended-upgrades）裝完後，needrestart 會把用到更新程式庫的服務重開；
# 遊戲與網頁橋接程式不要自動重開（會讓線上玩家斷線），由管理者自己選時間重開。
if [ -d /etc/needrestart/conf.d ]; then
    sudo tee /etc/needrestart/conf.d/es2-webmud.conf >/dev/null <<'NREOF'
# 自動更新後不要自動重開遊戲與網頁橋接程式，由管理者自己選時間重開
$nrconf{override_rc}{qr(^es2-mud\.service$)} = 0;
$nrconf{override_rc}{qr(^es2-web\.service$)} = 0;
NREOF
fi
echo "==== 4/4 清除編譯快取並重開 ===="
rm -rf "$MUD/bin/"*
sudo systemctl restart es2-mud
sleep 4
sudo systemctl restart es2-web
sleep 2
systemctl is-active es2-mud es2-web
echo "已更新到 $(git -C "$APP_DIR" log --oneline -1)"
