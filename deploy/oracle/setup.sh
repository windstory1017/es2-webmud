#!/bin/bash
# ES2 WebMUD 雲端主機一鍵安裝（Ubuntu 24.04，Oracle Cloud Always Free 的 Arm 或 AMD 主機皆可）。
# Neolith 需要 CMake 3.28 以上，Ubuntu 22.04 內建的 3.22 不夠，請建立 24.04 的主機。
#
# 用法（用 ssh 登入主機後執行）：
#   curl -fsSL https://raw.githubusercontent.com/wolfer168/es2-webmud/main/deploy/oracle/setup.sh -o setup.sh
#   bash setup.sh
#
# 可用環境變數調整：REPO_URL、BRANCH、APP_DIR、WEB_PORT。
# 重複執行是安全的：已經裝好的步驟會略過或更新。
set -euo pipefail

REPO_URL="${REPO_URL:-https://github.com/wolfer168/es2-webmud.git}"
BRANCH="${BRANCH:-main}"
APP_DIR="${APP_DIR:-$HOME/es2-webmud}"
WEB_PORT="${WEB_PORT:-8080}"
MUD_PORT=4001   # 與 source/upstream/neolith.conf 的 Port 一致；只給本機的網頁橋接程式用，不對外開放
RUN_USER="$(id -un)"
NEOLITH_BIN="$APP_DIR/source/upstream/neolith/out/build/linux/src/RelWithDebInfo/neolith"

step() { echo; echo "==== $* ===="; }

step "1/7 安裝系統套件"
. /etc/os-release
if [ "${ID:-}" != "ubuntu" ] || dpkg --compare-versions "${VERSION_ID:-0}" lt 24.04; then
    echo "需要 Ubuntu 24.04 以上（目前是 ${PRETTY_NAME:-未知}）。請重新建立主機時選 Canonical Ubuntu 24.04。"
    exit 1
fi
export DEBIAN_FRONTEND=noninteractive
sudo apt-get update -y
sudo apt-get install -y git build-essential cmake ninja-build bison pkg-config \
    libssl-dev libcurl4-openssl-dev libc-ares-dev curl ca-certificates nodejs npm
# 網頁橋接程式需要 Node.js 18 以上；版本太舊就改裝 NodeSource 的 Node.js 20。
if [ "$(node -p 'process.versions.node.split(".")[0]' 2>/dev/null || echo 0)" -lt 18 ]; then
    curl -fsSL https://deb.nodesource.com/setup_20.x | sudo -E bash -
    sudo apt-get install -y nodejs
fi

# 遊戲顯示的時間跟著主機時區；雲端主機預設是 UTC，改成台灣時間。
sudo timedatectl set-timezone "${TIMEZONE:-Asia/Taipei}" || true

step "2/7 記憶體不足 2GB 時加開 2GB swap（編譯時需要）"
MEM_MB=$(awk '/MemTotal/ {print int($2/1024)}' /proc/meminfo)
if [ "$MEM_MB" -lt 2000 ] && ! swapon --show | grep -q /swapfile; then
    sudo fallocate -l 2G /swapfile
    sudo chmod 600 /swapfile
    sudo mkswap /swapfile
    sudo swapon /swapfile
    grep -q '^/swapfile' /etc/fstab || echo '/swapfile none swap sw 0 0' | sudo tee -a /etc/fstab
else
    echo "記憶體 ${MEM_MB}MB，略過。"
fi

step "3/7 下載程式（$REPO_URL，分支 $BRANCH）"
if [ -d "$APP_DIR/.git" ]; then
    git -C "$APP_DIR" fetch origin "$BRANCH"
    git -C "$APP_DIR" checkout -B "$BRANCH" "origin/$BRANCH"
else
    git clone --branch "$BRANCH" "$REPO_URL" "$APP_DIR"
fi

step "4/7 編譯 Neolith（第一次約 5～15 分鐘，1GB 主機約 20～30 分鐘）"
# 記憶體不足 2GB 時一次只編譯一個檔案，避免記憶體用光而中斷。
[ "$MEM_MB" -lt 2000 ] && export CMAKE_BUILD_PARALLEL_LEVEL="${CMAKE_BUILD_PARALLEL_LEVEL:-1}"
cd "$APP_DIR/source/upstream/neolith"
cmake --preset linux
cmake --build --preset pr-linux --target neolith
test -x "$NEOLITH_BIN"

step "5/7 建立遊戲執行時需要的資料夾、安裝網頁橋接程式"
MUD="$APP_DIR/source/upstream/mudlib"
mkdir -p "$MUD/log/static" "$MUD/bin" "$MUD/data/board"   # log/static：自殺、複製物品等事件紀錄；data/board：留言板存檔（都不放在 git 裡）
for d in login user mail; do
    for c in a b c d e f g h i j k l m n o p q r s t u v w x y z; do
        mkdir -p "$MUD/data/$d/$c"
    done
done
cd "$APP_DIR/server"
npm ci --omit=dev
# 玩家版天朝圖鑑（web/codex.html）
python3 "$APP_DIR/tools/build_player_codex.py" || echo "（圖鑑產生失敗，之後執行 update.sh 會再產生一次）"

step "6/7 設定開機自動啟動（systemd）"
# 自動安全更新（unattended-upgrades）裝完後，needrestart 會把用到更新程式庫的服務重開；
# 遊戲與網頁橋接程式不要自動重開（會讓線上玩家斷線），由管理者自己選時間重開。
if [ -d /etc/needrestart/conf.d ]; then
    sudo tee /etc/needrestart/conf.d/es2-webmud.conf >/dev/null <<'NREOF'
# 自動更新後不要自動重開遊戲與網頁橋接程式，由管理者自己選時間重開
$nrconf{override_rc}{qr(^es2-mud\.service$)} = 0;
$nrconf{override_rc}{qr(^es2-web\.service$)} = 0;
NREOF
fi

sudo tee /etc/systemd/system/es2-mud.service >/dev/null <<EOF
[Unit]
Description=ES2 WebMUD - Neolith driver
After=network.target

[Service]
User=$RUN_USER
WorkingDirectory=$APP_DIR/source/upstream
ExecStart=$NEOLITH_BIN -f neolith.conf
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
EOF
sudo tee /etc/systemd/system/es2-web.service >/dev/null <<EOF
[Unit]
Description=ES2 WebMUD - web bridge
After=network.target es2-mud.service
Wants=es2-mud.service

[Service]
User=$RUN_USER
WorkingDirectory=$APP_DIR/server
Environment=HOST=0.0.0.0
Environment=PORT=$WEB_PORT
Environment=MUD_PORT=$MUD_PORT
ExecStart=$(command -v node) index.js
Restart=always
RestartSec=5

[Install]
WantedBy=multi-user.target
EOF
sudo systemctl daemon-reload
sudo systemctl enable es2-mud es2-web
sudo systemctl restart es2-mud
sleep 4
sudo systemctl restart es2-web

step "7/7 開放主機防火牆的 $WEB_PORT port（Oracle 的 Ubuntu 預設只開 22）"
if ! sudo iptables -C INPUT -p tcp --dport "$WEB_PORT" -m state --state NEW -j ACCEPT 2>/dev/null; then
    sudo iptables -I INPUT 1 -p tcp --dport "$WEB_PORT" -m state --state NEW -j ACCEPT
fi
if command -v netfilter-persistent >/dev/null; then
    sudo netfilter-persistent save
else
    echo "沒有 netfilter-persistent，防火牆規則重開機後需要重新執行本腳本。"
fi

sleep 3
echo
if curl -fsS "http://127.0.0.1:$WEB_PORT/api/health" >/dev/null; then
    IP=$(curl -fsS --max-time 5 https://ifconfig.me 2>/dev/null || echo "主機的公開 IP")
    echo "完成！版本：$(git -C "$APP_DIR" log --oneline -1)"
    echo "玩家網址：http://$IP:$WEB_PORT/"
    echo "（如果連不上，請確認 Oracle 主控台的 Security List 已開放 TCP $WEB_PORT）"
else
    echo "網頁橋接程式沒有回應，請執行：sudo journalctl -u es2-web -u es2-mud -n 50"
    exit 1
fi
