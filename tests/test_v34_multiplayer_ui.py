from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]

def text(p): return (ROOT/p).read_text(encoding='utf-8')

def test_landing_has_no_duplicate_html_title():
    s=text('web/index.html')
    assert 'landing-title' not in s
    assert 'landing-subtitle' not in s
    assert 'landing-enter-target' not in s

def test_hud_is_polled_from_canonical_hp_command():
    s=text('web/app.js')
    # 現在用 webhud（唯讀狀態查詢，內含 hp）輪詢：戰鬥中 0.8 秒、平常 2.5 秒
    assert "const HUD_POLL_MS = 800" in s
    assert "const HUD_IDLE_POLL_MS = 2500" in s
    assert "observer.begin('hp','webhud')" in s
    assert "ws.send((extra?'webhud '+extra.cmd:'webhud')+'\\r\\n')" in s
    assert 'currentRoomId' in s and 'startHudPolling()' in s
    assert 'isHpSummaryLine' in s

def test_multiplayer_bridge_has_capacity_heartbeat_and_private_session_details():
    s=text('server/index.js')
    assert 'MAX_SESSIONS' in s
    assert "ws.isAlive=true" in s
    assert "ws.on('pong'" in s
    assert "ws.terminate()" in s
    assert "'/api/online-count'" in s
    assert "'/api/sessions'" in s and "localhost only" in s
    assert 'tcp.setNoDelay(true)' in s

def test_each_ws_still_creates_its_own_tcp():
    s=text('server/index.js')
    ws=s.index("wss.on('connection'")
    tcp=s.index('net.createConnection',ws)
    assert ws >= 0 and tcp > ws
