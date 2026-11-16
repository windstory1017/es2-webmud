from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
APP=(ROOT/'web/app.js').read_text(encoding='utf-8')
BRIDGE=(ROOT/'server/index.js').read_text(encoding='utf-8')

def test_fresh_socket_clears_stale_room_before_any_hud_poll():
    open_block=APP.split("ws.onopen=()=>{",1)[1].split("};",1)[0]
    assert 'stopHudPolling()' in open_block
    assert 'currentRoomId=null' in open_block
    assert "tail=''" in open_block
    assert open_block.index('currentRoomId=null') < open_block.index("conn.textContent='LIVE'")

def test_closed_socket_also_forgets_room_identity():
    close_line=next(x for x in APP.splitlines() if "ws.onclose=e=>" in x)
    assert 'stopHudPolling()' in close_line
    assert 'currentRoomId=null' in close_line
    assert 'runtimeSnapshot=null' in close_line

def test_hud_poll_still_requires_a_current_room():
    # 一般輪詢一定要有目前房間；只有開機找房間（allowNoRoom）時才例外
    assert "if((!currentRoomId&&!allowNoRoom)||input.type==='password'||ws?.readyState!==1||hudPollInFlight)return false;" in APP
    assert "pollHud(force=false,allowNoRoom=false)" in APP.replace('function ','')

def test_bridge_diagnostics_log_packet_lengths_not_payload_content():
    assert 'WS->MUD packet bytes=${b.length} total=${row.bytesIn}' in BRIDGE
    # Do not dump m.toString(), Buffer contents, or user text into diagnostics.
    block=BRIDGE.split("ws.on('message',m=>{",1)[1].split("});",1)[0]
    assert 'm.toString' not in block
    assert 'b.toString' not in block
