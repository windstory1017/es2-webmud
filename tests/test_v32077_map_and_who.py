from pathlib import Path
import json,re
ROOT=Path(__file__).resolve().parents[1]
APP=(ROOT/'web/app.js').read_text(encoding='utf-8')
WHO=(ROOT/'source/upstream/mudlib/cmds/usr/who.c').read_text(encoding='utf-8')
HP=(ROOT/'source/upstream/mudlib/cmds/usr/hp.c').read_text(encoding='utf-8')
WHERE=(ROOT/'source/upstream/mudlib/cmds/imm/where.c').read_text(encoding='utf-8')
GRAPH=json.loads((ROOT/'web/custom_map_graph.json').read_text(encoding='utf-8'))

def test_version():
    assert (ROOT/'VERSION').read_text().strip().startswith('3.20')

def test_movement_forces_immediate_map_refresh_path():
    assert 'let mapRefreshPending = false;' in APP
    assert 'function hudPollReady(force=false,allowNoRoom=false)' in APP
    assert 'if(!urgent && Date.now()-lastUserCommandAt<HUD_USER_GRACE_MS)return false;' in APP
    assert 'function pollHud(force=false,allowNoRoom=false)' in APP
    assert 'pollHud(force)' in APP
    assert 'const forceMapRefresh=mapRefreshPending;' in APP
    assert 'kickHudAfterServerText(forceMapRefresh);' in APP
    for cmd in ['north','south','east','west','northeast','northwest','southeast','southwest','up','down','go','enter','out']:
        assert cmd in APP

def test_runtime_room_label_is_authoritative_even_for_seed_nodes():
    # V7（唯讀世界）取代了舊的「執行中房間名稱覆蓋地圖」：執行中的資料不能新增、改名或改動地圖房間，
    # 房間名稱一律以打包好的地圖資料庫為準。
    assert 'V7: runtime must never create, rename or otherwise mutate map rooms.' in APP
    assert 'return nodeById.has(id)?id:null;' in APP
    assert 'n.label=authoritativeLabel;' not in APP


def test_who_functionality_stays_present():
    for token in ['users()', 'case "-l":', 'case "-w":', 'case "-i":', 'case "-n":', 'start_more_if_needed', 'query_load_average()']:
        assert token in WHO

def test_old_neolith_trailing_call_commas_removed():
    pat=re.compile(r',\s*\n\s*\);')
    assert not pat.search(WHO)
    assert not pat.search(HP)
    assert not pat.search(WHERE)
