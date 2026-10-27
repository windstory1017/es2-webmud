#!/usr/bin/env python3
"""開 PR 自動檢查：開機、新建角色登入、舊角色再登入一次（docs/CI_AUTO_CHECK.md）。

用法：python3 tools/ci_boot_login.py --neolith <編好的 neolith 執行檔>

只在用完即丟的檢查機器上跑：會在 mudlib 底下建立 log、data 等資料夾，
並留下測試角色 citest 的存檔。任何一步失敗就印出最後收到的畫面並以非 0 結束。
"""
from pathlib import Path
import argparse, re, socket, subprocess, sys, time

ROOT = Path(__file__).resolve().parents[1]
UPSTREAM = ROOT / 'source/upstream'
MUD = UPSTREAM / 'mudlib'
PORT = 4001          # neolith.conf 只接受這個連線埠
BOOT_TIMEOUT = 60
STEP_TIMEOUT = 15
USER_ID, PASSWORD, NAME = 'citest', 'ci-check-123', '測試'

# 開機或登入時寫進 debug.log 代表有程式出錯的字樣
DEBUG_ERROR = re.compile(r'\.c:\d+: |Error|error in|Cannot load|failed to load', re.I)
TELNET = re.compile(rb'\xff[\xfb-\xfe].|\xff[\xf0-\xfa]')
ANSI = re.compile(r'\x1b\[[0-9;?]*[A-Za-z]')


class Fail(Exception):
    pass


def prepare_dirs():
    """照 deploy/oracle/setup.sh 建好執行時要用的資料夾（git 裡沒有）。"""
    for d in ('log/static', 'bin', 'data/board'):
        (MUD / d).mkdir(parents=True, exist_ok=True)
    for d in ('login', 'user', 'mail'):
        for c in 'abcdefghijklmnopqrstuvwxyz':
            (MUD / 'data' / d / c).mkdir(parents=True, exist_ok=True)


class Session:
    def __init__(self):
        self.sock = socket.create_connection(('127.0.0.1', PORT), timeout=5)
        self.sock.settimeout(0.3)
        self.raw = b''
        self.seen = 0

    def text(self):
        return ANSI.sub('', TELNET.sub(b'', self.raw).decode('utf-8', 'replace'))

    def expect(self, *needles, step):
        """等到畫面（從上一次 expect 之後）出現全部字樣。"""
        end = time.time() + STEP_TIMEOUT
        while True:
            new = self.text()[self.seen:]
            if all(n in new for n in needles):
                last = max(new.rfind(n) + len(n) for n in needles)
                self.seen += last
                return new
            if time.time() > end:
                raise Fail(f'{step}：{STEP_TIMEOUT} 秒內沒看到 {needles}\n'
                           f'---- 最後收到的畫面 ----\n{new[-1500:]}')
            try:
                data = self.sock.recv(65536)
                if not data:
                    raise Fail(f'{step}：遊戲中斷了連線\n---- 最後收到的畫面 ----\n{new[-1500:]}')
                self.raw += data
            except socket.timeout:
                pass

    def send(self, line):
        self.sock.sendall((line + '\r\n').encode('utf-8'))

    def close(self):
        self.sock.close()


def wait_for_port(proc):
    end = time.time() + BOOT_TIMEOUT
    while time.time() < end:
        if proc.poll() is not None:
            raise Fail(f'遊戲開機途中結束了（結束代碼 {proc.returncode}）')
        try:
            socket.create_connection(('127.0.0.1', PORT), timeout=1).close()
            return
        except OSError:
            time.sleep(1)
    raise Fail(f'{BOOT_TIMEOUT} 秒內連不上 {PORT} 連線埠')


def new_player():
    s = Session()
    s.expect('您的使用者代號', step='連線畫面')
    s.send(USER_ID)
    s.expect('創造一個新的人物', step='輸入新代號')
    s.send('y')
    s.expect('請設定您的密碼', step='確認新人物')
    s.send(PASSWORD)
    s.expect('請再輸入一次您的密碼', step='設定密碼')
    s.send(PASSWORD)
    s.expect('您的電子郵件地址', step='再次輸入密碼')
    s.send('none')
    s.expect('人類 (human)', '您的選擇', step='輸入電子郵件')
    s.send('human')
    s.expect('(Male / Female)', step='選種族')
    s.send('m')
    s.expect('您的顯示名稱', step='選性別')
    s.send(NAME)
    s.expect('您確定要創造這個角色嗎', step='輸入顯示名稱')
    s.send('y')
    s.expect('冒險開始了', '目前權限', '客棧大堂', '> ', step='創造角色後進入遊戲')
    s.send('score')
    s.expect(f'{NAME} (Citest)', '形體', '> ', step='score')
    s.send('quit')
    s.expect('歡迎下次再來', step='quit')
    s.close()


def old_player():
    s = Session()
    s.expect('您的使用者代號', step='（舊角色）連線畫面')
    s.send(USER_ID)
    s.expect('請輸入密碼', step='（舊角色）輸入代號')
    s.send(PASSWORD)
    s.expect('目前權限', '客棧大堂', '> ', step='（舊角色）輸入密碼後進入遊戲')
    s.send('look')
    s.expect('客棧大堂', '> ', step='（舊角色）look')
    s.send('quit')
    s.expect('歡迎下次再來', step='（舊角色）quit')
    s.close()


def log_problems():
    """開機與登入期間程式出錯的紀錄。"""
    problems = []
    debug = MUD / 'log/debug.log'
    if debug.exists():
        for line in debug.read_text(encoding='utf-8', errors='replace').splitlines():
            if 'process terminated' in line:    # 本檢查自己送 SIGTERM 關伺服器，之後的收尾紀錄不算
                break
            if DEBUG_ERROR.search(line):
                problems.append(f'log/debug.log：{line}')
    compile_log = MUD / 'log/log'    # master.c 的 log_error 寫在這裡
    if compile_log.exists() and compile_log.read_text(encoding='utf-8', errors='replace').strip():
        problems += ['log/log：' + x for x in compile_log.read_text(encoding='utf-8', errors='replace').splitlines()]
    return problems


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--neolith', required=True, help='編好的遊戲伺服器執行檔')
    a = ap.parse_args()
    if (MUD / 'data/login/c' / f'{USER_ID}.o').exists():
        print(f'已有 {USER_ID} 的存檔，這個檢查只在全新的檢查機器上跑。', file=sys.stderr)
        return 2

    prepare_dirs()
    proc = subprocess.Popen([str(Path(a.neolith).resolve()), '-f', 'neolith.conf'],
                            cwd=UPSTREAM, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    ok = False
    try:
        t = time.time()
        wait_for_port(proc)
        print(f'開機：OK（{time.time() - t:.1f} 秒）')
        new_player()
        print('新角色建立、score、quit：OK')
        old_player()
        print('舊角色再登入、look、quit：OK')
        ok = True
    except Fail as e:
        print(f'失敗：{e}', file=sys.stderr)
    finally:
        proc.terminate()
        try:
            out, _ = proc.communicate(timeout=10)
        except subprocess.TimeoutExpired:
            proc.kill()
            out, _ = proc.communicate()
    problems = log_problems()
    if problems:
        print('失敗：紀錄檔裡有程式錯誤：', file=sys.stderr)
        for p in problems[:50]:
            print('  ' + p, file=sys.stderr)
        ok = False
    if not ok:
        print('---- 遊戲伺服器輸出（最後 60 行）----', file=sys.stderr)
        print('\n'.join(out.decode('utf-8', 'replace').splitlines()[-60:]), file=sys.stderr)
        return 1
    print('開機與登入檢查全部通過')
    return 0


if __name__ == '__main__':
    sys.exit(main())
