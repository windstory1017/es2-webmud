# 把 ES2 WebMUD 架到 Oracle Cloud（Always Free）

這份說明帶你從申請帳號開始，把遊戲架到一台 24 小時開著的雲端主機上，玩家用瀏覽器打開
`http://主機IP:8080/` 就能玩。安裝和更新都已經寫成腳本（`deploy/oracle/`），你只需要在網頁上點幾下、
在主機上貼兩行指令。

> Oracle 的網頁介面偶爾會改版，按鈕名稱可能和這裡寫的略有不同，找意思相近的就可以。

## 一、申請帳號

1. 打開 <https://www.oracle.com/cloud/free/>，按「Start for free」。
2. 填寫資料，**Home Region（主要地區）選了之後不能改**，建議選離台灣近的：
   - Japan East (Tokyo)、Japan Central (Osaka)、South Korea Central (Seoul) 或 Singapore。
3. 需要綁信用卡驗證身分。只要不按「Upgrade」升級成付費帳號，Always Free 的資源不會收費。
4. 開通可能要等幾分鐘到幾小時，收到信後登入 <https://cloud.oracle.com/>。

## 二、建立主機

1. 左上角選單 → **Compute** → **Instances** → **Create instance**。
2. **Name**：隨意，例如 `es2-webmud`。
3. **Image**：按「Edit」→「Change image」→ 選 **Canonical Ubuntu**，版本選 **24.04**。
   - 一定要 24.04，22.04 的 CMake 版本太舊，編譯會失敗。
4. **Shape**：按「Change shape」：
   - 優先選 **Ampere** → **VM.Standard.A1.Flex**，設 **2 OCPU、12 GB 記憶體**（標示 Always Free-eligible）。
   - 如果按 Create 時出現 **Out of capacity**，先試著把 OCPU 降成 1（記憶體 6 GB）；還是不行就晚點再試，或改選
     **Specialty and previous generation** → **VM.Standard.E2.1.Micro**（AMD，1 GB 記憶體，也能跑；安裝腳本會自動加開 swap，
     並改成一次只編譯一個檔案，編譯約 20～30 分鐘）。
5. **Networking**：用預設的「Create new virtual cloud network」和「Create new public subnet」，並確認
   「**Assign a public IPv4 address**」是勾選的。
   - 新版介面是分步驟的（Basic information → Security → Networking → Storage → Review），Security、Storage 維持預設即可。
   - 如果 Public IPv4 的開關按不了（提示 You must select a public subnet），先另開分頁：**Networking → Virtual cloud networks →
     Start VCN Wizard → Create VCN with Internet Connectivity**，名稱填 `es2-vcn`、其他預設建立；再回來選
     「Select existing virtual cloud network」→ `es2-vcn`，子網路選名稱有 public 的那個。
6. **Add SSH keys**：選「**Generate a key pair for me**」，按「**Save private key**」把私鑰存到電腦上
   （例如 `C:\Users\你的名字\.ssh\es2.key`）。**這個檔案遺失就登入不了主機，請妥善保存。**
7. 按「**Create**」。等狀態變成綠色的 **Running**，記下頁面上的 **Public IP address**。

## 三、開放 8080 port

Oracle 預設只開放 SSH（22）。玩家要從外面連進來，需要開放 8080：

1. 在主機頁面點 **Primary VNIC** 底下的 **Subnet** 名稱。
2. 進入 **Security Lists** → 點預設的那一個（Default Security List…）。
3. 按「**Add Ingress Rules**」，填：
   - Source CIDR：`0.0.0.0/0`
   - IP Protocol：TCP
   - Destination Port Range：`8080`
4. 按「Add Ingress Rules」儲存。

> MUD 本體的 4001 port 只給主機內部的網頁橋接程式用，不需要也不要對外開放。

### 另外開放 zMUD／Telnet 直連（選用）

想讓玩家用 zMUD 或其他 MUD 用戶端直接連線，照同樣的步驟再新增兩條規則（Source `0.0.0.0/0`、TCP）：

| Destination Port | 用途 |
|---|---|
| `4000` | zMUD 4.62（Big5 繁體） |
| `4003` | Mudlet、MUSHclient、CMUD、PuTTY 等（UTF-8） |

主機防火牆由 `setup.sh` / `update.sh` 自動開放。想關閉某個入口：在 `es2-web` 服務設定
`ZMUD_PORT=0` 或 `TELNET_PORT=0`（`sudo systemctl edit es2-web` 加 `[Service]` 與 `Environment=ZMUD_PORT=0`），再 `sudo systemctl restart es2-web`。

## 四、連上主機

在 Windows 打開 **PowerShell**：

```powershell
ssh -i C:\Users\你的名字\.ssh\es2.key ubuntu@主機的公開IP
```

第一次會問是否信任主機，輸入 `yes`。如果出現「UNPROTECTED PRIVATE KEY FILE」錯誤，先執行下面三行修正私鑰權限再連一次：

```powershell
icacls C:\Users\你的名字\.ssh\es2.key /inheritance:r
icacls C:\Users\你的名字\.ssh\es2.key /grant:r "$($env:USERNAME):R"
ssh -i C:\Users\你的名字\.ssh\es2.key ubuntu@主機的公開IP
```

看到 `ubuntu@es2-webmud:~$` 就是登入成功了。

## 五、安裝遊戲

在主機上貼這兩行：

```bash
curl -fsSL https://raw.githubusercontent.com/wolfer168/es2-webmud/main/deploy/oracle/setup.sh -o setup.sh
bash setup.sh
```

腳本會依序：安裝套件 → 下載最新的 main → 編譯 Neolith → 建立資料夾 → 設定開機自動啟動 → 開放主機防火牆的 8080。
第一次大約 10～20 分鐘（主要花在編譯）。最後會顯示：

```
完成！版本：xxxxxxx ...
玩家網址：http://主機IP:8080/
```

用瀏覽器打開這個網址就能進遊戲。

- 這是全新的伺服器，**角色要重新建立**，你電腦上的角色不會帶過來。
- 腳本可以重複執行，中途失敗修正後再跑一次即可。

## 六、之後更新

GitHub 的 main 更新後，登入主機執行：

```bash
bash ~/es2-webmud/deploy/oracle/update.sh
```

它會先把玩家資料備份到 `~/es2-backups/`（保留最近 14 份），再拉最新的 main、必要時重新編譯，清除快取後重開。

## 七、常用指令

| 用途 | 指令 |
|---|---|
| 看狀態 | `systemctl status es2-mud es2-web` |
| 重開 | `sudo systemctl restart es2-mud es2-web` |
| 停止 | `sudo systemctl stop es2-web es2-mud` |
| 看最近的錯誤訊息 | `sudo journalctl -u es2-mud -u es2-web -n 50` |
| 看 MUD 本身的記錄 | `ls ~/es2-webmud/source/upstream/mudlib/log/` |

## 八、注意事項

- **公開範圍**：網址任何人都能連。網頁橋接程式的管理功能（重建目錄、還原備份、編輯原始碼）只接受主機本機的連線，外面連不到。
- **備份**：玩家存檔在 `~/es2-webmud/source/upstream/mudlib/data/`。除了更新時的自動備份，偶爾也可以用 `scp` 下載 `~/es2-backups/` 到自己電腦。
- **閒置回收**：Oracle 可能回收長期幾乎沒有使用的免費主機。MUD 一直在跑通常不會被判定閒置，但不是絕對保證，所以備份很重要。
- **網址與 HTTPS**：見下一節。

## 九、改成 HTTPS

1. Oracle 主控台 → 子網路的 **Security List** → **Add Ingress Rules**，新增兩條：Source `0.0.0.0/0`、TCP、Destination Port `80`；
   另一條一樣但 Port 填 `443`。（80 是申請憑證時驗證用的，不能省。）
2. 在主機上執行：
   ```bash
   bash ~/es2-webmud/deploy/oracle/https.sh
   ```
   沒有網域時會用 `sslip.io` 這個免費服務，網址長得像 `https://161-118-234-100.sslip.io/`，只要主機 IP 不變就一直有效。
   之後買了自己的網域，把 DNS 的 A 記錄指到主機 IP，再執行 `bash ~/es2-webmud/deploy/oracle/https.sh 你的網域` 即可。
3. 憑證由 Caddy 自動申請、自動續約，不用管。原本的 `http://IP:8080/` 仍可使用；確定大家都改用 HTTPS 後，
   可以把 Security List 的 8080 規則刪掉。

> 網頁橋接程式會把帶有轉送標頭（Caddy、Cloudflare Tunnel 都會加）的請求視為外部連線，所以管理功能不會因為加了 HTTPS 而對外開放。

## 十、新內容的上線流程

1. Claude 在開發分支 `claude/epic-ramanujan-wuyis3` 上修改並推送。
2. 家裡電腦（WSL）先測試：
   ```bash
   curl -fsSL https://raw.githubusercontent.com/gbfangyans-star/es2-webmud/claude/epic-ramanujan-wuyis3/deploy/local/try_branch.sh | bash
   ```
   打開 `http://127.0.0.1:8080/` 試玩。有問題就回報 Claude 修改，修好後再跑一次同一行。
3. 確認沒問題後，用合併連結開 PR 並合併進 main：
   <https://github.com/wolfer168/es2-webmud/compare/main...gbfangyans-star:es2-webmud:claude/epic-ramanujan-wuyis3>
4. 登入雲端主機執行 `bash ~/es2-webmud/deploy/oracle/update.sh`。
5. 家裡電腦想切回正式版：把第 2 步那行最後的 `| bash` 改成 `| bash -s main`。
