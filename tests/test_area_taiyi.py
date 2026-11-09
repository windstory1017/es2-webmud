"""太乙觀（d/taiyi）：房間、出口、指令出入、NPC 與敘述排版（docs/AREA_TAIYI.md）。"""
from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'
T = MUD / 'd' / 'taiyi'
OPP = {'north': 'south', 'south': 'north', 'east': 'west', 'west': 'east', 'northeast': 'southwest',
       'southwest': 'northeast', 'northwest': 'southeast', 'southeast': 'northwest'}
NO_LINE_START = '﹗！？﹖，﹐。﹒、；﹔：﹕」』）】〉》…'
OLD_TEXT = {'guest1', 'guest2', 'guest3'}   # 舊資料全文引用（賓客宿房）


def read(p):
    return p.read_text(encoding='utf-8')


def exits(name):
    m = re.search(r'set\("exits", \(\[(.*?)\]\)\);', read(T / (name + '.c')), re.S)
    return dict(re.findall(r'"([a-z]+)" : (?:__DIR__)?"([^"]+)"', m.group(1))) if m else {}


def long_text(name):
    return re.search(r'@LONG\n(.*?)\nLONG', read(T / (name + '.c')), re.S).group(1)


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


WANT = {
    'gate': {'northwest': 'hall', 'east': '/d/graveyard/grave8'},
    'hall': {'southeast': 'gate', 'west': 'corridor1', 'east': 'main_hall'},
    'main_hall': {'west': 'hall', 'east': 'cell', 'north': 'yard'},
    'cell': {'west': 'main_hall'},
    'corridor1': {'east': 'hall', 'north': 'corridor2', 'west': 'guest3'},
    'corridor2': {'south': 'corridor1', 'north': 'corridor3', 'west': 'guest2'},
    'corridor3': {'south': 'corridor2', 'west': 'guest1', 'northeast': 'trail1'},
    'guest1': {'east': 'corridor3'},
    'guest2': {'east': 'corridor2'},
    'guest3': {'east': 'corridor1'},
    'yard': {'south': 'main_hall'},
    'well': {},
    'passage1': {'south': 'well', 'north': 'passage2'},
    'passage2': {'south': 'passage1', 'east': 'passage3'},
    'passage3': {'west': 'passage2', 'east': 'cliff'},
    'cliff': {'west': 'passage3'},
    'trail1': {'southwest': 'corridor3', 'north': 'trail2'},
    'trail2': {'south': 'trail1', 'northwest': 'trail3'},
    'trail3': {'southeast': 'trail2'},
}


def test_rooms_and_exits():
    assert sorted(p.stem for p in T.glob('*.c')) == sorted(WANT)
    for r, ex in WANT.items():
        assert exits(r) == ex, r
        assert 'set("map/area", "太乙觀");' in read(T / (r + '.c'))
    # 一般方向出口都能走回來（通道往南回井底、井底沒有一般出口，是單向）。
    for r, ex in WANT.items():
        for d, t in ex.items():
            if t.startswith('/') or (r, d) == ('passage1', 'south'):
                continue
            assert exits(t).get(OPP[d]) == r, (r, d, t)


def test_entrance_from_graveyard():
    assert '"west" : "/d/taiyi/gate"' in read(MUD / 'd/graveyard/grave8.c')


def test_doors_closed():
    for corridor, guest in (('corridor1', 'guest3'), ('corridor2', 'guest2'), ('corridor3', 'guest1')):
        assert 'create_door("west", "廂房木門", "east", DOOR_CLOSED);' in read(T / (corridor + '.c'))
        assert 'create_door("east", "廂房木門", "west", DOOR_CLOSED);' in read(T / (guest + '.c'))


def test_command_exits():
    s = read(T / 'yard.c')
    assert 'add_action("do_climb", "climb");' in s and 'arg != "繩索"' in s and 'me->move(__DIR__"well")' in s
    s = read(T / 'well.c')
    assert 'add_action("do_climb", "climb");' in s and 'me->move(__DIR__"yard")' in s
    assert 'add_action("do_knock", "knock");' in s and 'arg != "井壁"' in s
    assert 'add_action("do_push", "push");' in s and 'me->move(__DIR__"passage1")' in s
    # 暗門找到之後房間記住，直到重置（使用者定案，2026-11-09）。
    assert 'set_temp("door_found", 1);' in s and 'delete_temp("door_found");' in s
    assert 'if( dir == "south" )' in read(T / 'passage1.c')


def test_npcs_placed():
    want = {
        'gate': ['"/d/baihua/npc/stone_lion" : 2'],
        'main_hall': ['"/custom/legacy_npc/taoist_leader" : 1'],
        'cell': ['__DIR__"npc/shang_ching" : 1'],
        'cliff': ['__DIR__"npc/hsuan_ching" : 1'],
        'guest1': ['__DIR__"npc/pilgrim" : 1'],
        'guest3': ['__DIR__"npc/pilgrim" : 1'],
    }
    for r, objs in want.items():
        for o in objs:
            assert o in read(T / (r + '.c')), (r, o)
    assert 'set("age", 45);' in read(T / 'npc/shang_ching.c')
    assert 'set("age", 45);' in read(T / 'npc/hsuan_ching.c')
    assert '"shang-ching"' in read(T / 'npc/shang_ching.c') and '"hsuan-ching"' in read(T / 'npc/hsuan_ching.c')
    pil = read(T / 'npc/pilgrim.c')
    assert 'set_level(5);' in pil and 'set_power("C");' in pil and 'carry_money("silver", 40 + random(11));' in pil


def test_masters():
    s = read(MUD / 'custom/legacy_npc/taoist_leader.c')
    assert '#define FACTION "taoist.freeze"' in s
    assert 'if( me->query_class() != "commoner" )' in s and '素衣派弟子' in s
    for sk in ('taoism-freeze', 'spells', 'sword', 'dodge', 'parry', 'polar-silkworm force'):
        assert f'case "{sk}":' in s, sk
    assert 'me->query_level() < 15' in s
    s = read(T / 'npc/shang_ching.c')
    assert 'skill != "five-steps"' in s and '#define FACTION "taoist.freeze"' in s


def test_description_layout():
    hint = re.compile(r'[（(][^（）()\n]{1,14}[）)]')
    for p in T.glob('*.c'):
        text = long_text(p.stem)
        assert not hint.search(text), p.name
        lines = wrap(text)
        for line in lines:
            assert sum(2 if len(c.encode()) > 1 else 1 for c in line) <= 70, (p.name, line)   # 標點不凸出
        if p.stem not in OLD_TEXT:
            assert 3 <= len(lines) <= 4 and len(lines[-1]) >= 6, (p.name, lines)
    assert long_text('cliff').endswith('頗有獨上高樓，望盡天涯路之感。')


def test_web_map():
    g = json.loads(read(ROOT / 'web/world_static_map.json'))
    ids = [n for n in g['nodes'] if n['id'].startswith('/d/taiyi/')]
    assert len(ids) == 19 and {n['area'] for n in ids} == {'太乙觀'}
    assert len({(n['x'], n['y'], n.get('layer')) for n in ids}) == 19
    assert {n['id'] for n in ids if n.get('layer') == '地下'} == {'/d/taiyi/' + r for r in ('well', 'passage1', 'passage2', 'passage3')}
    assert any(e['from'] == '/d/graveyard/grave8' and e['to'] == '/d/taiyi/gate' for e in g['edges'])
    assert any(e['from'] == '/d/taiyi/gate' and e['to'] == '/d/graveyard/grave8' for e in g['edges'])


if __name__ == '__main__':
    for name, fn in list(globals().items()):
        if name.startswith('test_') and callable(fn):
            fn()
            print('ok', name)
