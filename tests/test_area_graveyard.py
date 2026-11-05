"""亂葬崗（d/graveyard）：房間、出口、指令出入、NPC 與敘述排版（docs/AREA_GRAVEYARD.md）。"""
from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'
G = MUD / 'd' / 'graveyard'
OPP = {'north': 'south', 'south': 'north', 'east': 'west', 'west': 'east', 'northeast': 'southwest',
       'southwest': 'northeast', 'northwest': 'southeast', 'southeast': 'northwest', 'out': None}
NO_LINE_START = '﹗！？﹖，﹐。﹒、；﹔：﹕」』）】〉》…'
OLD_TEXT = {'shrine', 'grave5', 'grave8', 'weeds'}   # 舊資料全文引用


def read(p):
    return p.read_text(encoding='utf-8')


def exits(name):
    m = re.search(r'set\("exits", \(\[(.*?)\]\)\);', read(G / (name + '.c')), re.S)
    return dict(re.findall(r'"([a-z]+)" : (?:__DIR__)?"([^"]+)"', m.group(1))) if m else {}


def long_text(name):
    return re.search(r'@LONG\n(.*?)\nLONG', read(G / (name + '.c')), re.S).group(1)


def wrap(s, width=70, first=4):
    """照 adm/simul_efun/utf8.c 的 cjk_wrap() 換行（房間敘述第一行空四格）。"""
    lines, out, n, prev = [], ' ' * first, first, 0
    for ch in s:
        w = 2 if len(ch.encode()) > 1 else 1
        if ch == '\n':
            if prev > 1:
                continue
            ch = ' '
        prev = w
        if (w > 1 or ch == ' ') and n + w > width and ch not in NO_LINE_START:
            lines.append(out)
            out, n = '', 0
        if ch == ' ' and n == 0:
            continue
        out += ch
        n += w
    return lines + [out]


def test_rooms_and_exits():
    want = {
        'grave1': {'west': 'grave2', 'southeast': '/d/snow/wgate'},
        'grave2': {'east': 'grave1', 'northwest': 'grave3'},
        'grave3': {'northwest': 'grave4', 'southeast': 'grave2'},
        'grave4': {'west': 'grave5', 'southeast': 'grave3'},
        'grave5': {'southwest': 'grave6', 'east': 'grave4'},
        'shrine': {'out': 'grave5'},
        'grave6': {'northeast': 'grave5', 'south': 'grave7'},
        'grave7': {'north': 'grave6', 'west': 'grave8'},
        'grave8': {'east': 'grave7'},
        'weeds': {'north': 'grave8', 'south': 'forest'},
        'forest': {'north': 'weeds', 'west': 'forest_w', 'east': 'forest_e'},
        'forest_w': {'east': 'forest'},
        'forest_e': {'west': 'forest'},
    }
    assert sorted(p.stem for p in G.glob('*.c')) == sorted(want)
    for r, ex in want.items():
        assert exits(r) == ex, r
        assert 'set("map/area", "亂葬崗");' in read(G / (r + '.c'))
    for r in ('forest', 'forest_w', 'forest_e'):
        assert 'set("short", "密林");' in read(G / (r + '.c'))
    # 一般方向出口都能走回來（雜草叢往北回小路是單向，小路要撥開草叢才進得去）。
    for r, ex in want.items():
        for d, t in ex.items():
            if OPP.get(d) and not t.startswith('/') and r != 'weeds':
                assert exits(t).get(OPP[d]) == r, (r, d, t)


def test_entrance_from_snow_west_gate():
    assert '"northwest" : "/d/graveyard/grave1"' in read(MUD / 'd/snow/wgate.c')


def test_command_exits():
    s = read(G / 'grave5.c')
    assert 'add_action("do_enter", "enter");' in s and 'if( arg != "小祠" ) return 0;' in s
    assert 'me->move(__DIR__"shrine")' in s
    s = read(G / 'grave8.c')
    assert 'add_action("do_pass", "pass");' in s and 'if( arg != "草叢" ) return 0;' in s
    assert 'me->move(__DIR__"weeds")' in s


def test_npcs_placed():
    want = {
        'grave5': ['"/custom/legacy_npc/lady" : 1'],
        'shrine': ['__DIR__"npc/dirty_taoist" : 1'],
        'grave7': ['__DIR__"npc/old_bonze" : 1', '__DIR__"npc/young_bonze" : 1'],
        'forest_w': ['"/custom/legacy_npc/wu_changuei" : 1'],
        'forest_e': ['"/custom/legacy_npc/barbarian_chief" : 1', '__DIR__"npc/barbarian" : 2'],
    }
    for r, objs in want.items():
        for o in objs:
            assert o in read(G / (r + '.c')), (r, o)
    for f in re.findall(r'"(/d/graveyard/obj/[a-z_]+)"', ''.join(read(p) for p in (G / 'npc').glob('*.c'))):
        assert (MUD / (f[1:] + '.c')).exists(), f
    # 酋長要比野人強（使用者指示，2026-11-04）。
    chief = read(MUD / 'custom/legacy_npc/barbarian_chief.c')
    bar = read(G / 'npc/barbarian.c')
    assert 'set_level(15);' in chief and 'set_power("A");' in chief
    assert 'set_level(12);' in bar and 'set_power("B");' in bar and 'set_attr("str", 20);' in bar


def test_items():
    ring = read(G / 'obj/chrysoberyl_ring.c')
    assert '"cps": 1,' in ring and '「雍順」' in ring
    staff = read(G / 'obj/brass_zenstaff.c')
    assert 'set_weight(18840);' in staff and 'set("value", 72000);' in staff
    assert '"wis": 1,' in staff and '"wittiness": 5,' in staff
    rosary = read(G / 'obj/rosary.c')
    assert 'set_weight(20);' in rosary and 'set("value", 1200);' in rosary and '"int": 1,' in rosary


def test_description_layout():
    hint = re.compile(r'[（(][^（）()\n]{1,14}[）)]')
    for p in G.glob('*.c'):
        text = long_text(p.stem)
        assert not hint.search(text), p.name
        lines = wrap(text)
        for line in lines:
            assert sum(2 if len(c.encode()) > 1 else 1 for c in line) <= 70, (p.name, line)   # 標點不凸出
        if p.stem not in OLD_TEXT:
            assert 3 <= len(lines) <= 4 and len(lines[-1]) >= 6, (p.name, lines)


def test_web_map():
    g = json.loads(read(ROOT / 'web/world_static_map.json'))
    ids = [n for n in g['nodes'] if n['id'].startswith('/d/graveyard/')]
    assert len(ids) == 13 and {n['area'] for n in ids} == {'亂葬崗'}
    assert len({(n['x'], n['y']) for n in ids}) == 13
    assert any(e['from'] == '/d/snow/wgate' and e['to'] == '/d/graveyard/grave1' for e in g['edges'])


if __name__ == '__main__':
    for name, fn in list(globals().items()):
        if name.startswith('test_') and callable(fn):
            fn()
            print('ok', name)
