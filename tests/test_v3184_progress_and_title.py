from pathlib import Path
R=Path(__file__).resolve().parents[1]

def test_version_and_preserved_live_characters():
 assert (R/'VERSION').read_text().strip().startswith('3.')
 # 玩家存檔（data/user、data/login）依 .gitignore 不放進 repo，不再檢查存檔檔案是否存在

def test_live_gate_has_visible_progress_and_bounded_waits():
 s=(R/'tools/live_multiplayer_resilience.py').read_text(encoding='utf-8')
 assert "progress('LOGIN: connecting both characters...')" in s
 assert 'LOGIN phase exceeded 40 seconds' in s
 assert "progress(f'WAVE {i}/{max(1,a.waves)}" in s
 assert 'deadline=time.monotonic()+4.25' in s
 assert 'sender.read(.10)' in s and 'recipient.read(.10)' in s
 assert 'concurrent save phase exceeded 6 seconds' in s

def test_login_title_is_plain_text_from_the_mud():
 # 連線後的標題改由遊戲送出的文字顯示，網頁不再換成圖片。
 app=(R/'web/app.js').read_text(encoding='utf-8')
 assert '/login_title_v3185.png' not in app
 assert 'login-welcome-art' not in app
