from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]

def text(rel): return (ROOT/rel).read_text(encoding='utf-8-sig' if rel.endswith('.ps1') else 'utf-8')

def test_skill_daemon_fallback_uses_object_safe_comparison():
    s=text('source/upstream/mudlib/feature/char/skill.c')
    # 技能檔現在一律先確認 objectp(daemon)，再用 function_exists 判斷有沒有該函式
    assert 'objectp(daemon) && function_exists(' in s
    assert 'if( daemon != DAEMON_D )' not in s

def test_launcher_requires_active_mud_handshake():
    s=text('start_es2_internet_windows.ps1')
    assert 'function Test-MudHandshake' in s
    assert '[void](Test-MudHandshake)' in s
    assert 'Neolith accepted TCP but sent no login/telnet greeting' in s

def test_bridge_records_tcp_close_reason_and_bytes():
    s=text('server/index.js')
    assert "reports','bridge_runtime.log" in s
    assert 'TCP closed hadError=' in s
    assert "ws.close(1011,'MUD TCP closed')" in s
