from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'
Q = MUD / 'd' / 'qiaoyin'
OPP = {'north': 'south', 'south': 'north', 'east': 'west', 'west': 'east', 'northeast': 'southwest',
       'southwest': 'northeast', 'northwest': 'southeast', 'southeast': 'northwest', 'up': 'down', 'down': 'up'}


def read(p):
    return p.read_text(encoding='utf-8')


def exits(path):
    m = re.search(r'set\("exits", \(\[(.*?)\]\)\);', read(path), re.S)
    out = {}
    for d, rel, ab in re.findall(r'"([a-z]+)" : (?:__DIR__"([^"]+)"|"(/[^"]+)")', m.group(1) if m else ''):
        out[d] = rel or ab
    return out


def test_rooms_and_npcs():
    rooms = sorted(p.stem for p in Q.glob('*.c'))
    assert len(rooms) == 35
    assert len(list((Q / 'npc').glob('*.c'))) == 14
    for r in rooms:
        s = read(Q / (r + '.c'))
        assert 'set("map/area", "喬陰縣");' in s, r
        for npc in re.findall(r'__DIR__"npc/([a-z_]+)"', s):
            assert (Q / 'npc' / (npc + '.c')).exists(), (r, npc)
        for ob in re.findall(r'"(/(?:custom|d|obj)/[^"]+)" : \d', s):
            assert (MUD / (ob.lstrip('/') + '.c')).exists(), (r, ob)


def test_exits_are_two_way_and_linked():
    for p in Q.glob('*.c'):
        for d, t in exits(p).items():
            if t.startswith('/'):
                continue
            assert exits(Q / (t + '.c')).get(OPP[d]) == p.stem, (p.stem, d, t)
    assert exits(Q / 'ngate.c')['north'] == '/d/oldpine/highway'
    assert exits(MUD / 'd/oldpine/highway.c')['south'] == '/d/qiaoyin/ngate'
    assert exits(Q / 'highway.c')['northwest'] == '/d/wutang/three_way'
    assert exits(MUD / 'd/wutang/three_way.c')['southeast'] == '/d/qiaoyin/highway'
    # 南門不開出口；東門主路封閉，只有城牆腳下的小徑通往東北（喬陰城外，2026-11-16）
    assert set(exits(Q / 'egate.c')) == {'west', 'northeast'} and set(exits(Q / 'sgate.c')) == {'north'}


def test_alchemy_room_gate():
    s = read(Q / 'big_hall.c')
    assert 'me->query_class() != "alchemist"' in s and 'query_temp("qiaoyin/alchemy_paid")' in s
    assert set(exits(Q / 'big_hall.c')) == {'north', 'east'}      # 大屋正廳取消 west
    j = read(Q / 'npc/jiang_hsuren.c')
    assert '#define ALCHEMY_FEE 500' in j and 'set_temp("qiaoyin/alchemy_paid", 1);' in j


def test_special_rooms():
    assert 'inherit BANK;' in read(Q / 'bank.c')
    assert 'inherit HOCKSHOP;' in read(Q / 'pawnshop.c')
    inn = read(Q / 'inn.c')
    assert 'inherit INN;' in inn and 'set("no_fight", 1);' in inn and 'set("valid_startroom", 1);' in inn
    assert '"石碑"' in read(Q / 'tree_tomb.c')


def test_no_command_hints_in_descriptions():
    hint = re.compile(r'[（(][^（）()\n]{1,14}[）)]')
    for p in list(Q.glob('*.c')) + list((Q / 'npc').glob('*.c')):
        for m in re.finditer(r'@LONG\n(.*?)\nLONG', read(p), re.S):
            assert not hint.search(m.group(1)), p.name
            assert 'list' not in m.group(1) and 'buy' not in m.group(1), p.name


def test_web_map():
    g = json.loads(read(ROOT / 'web/world_static_map.json'))
    ids = [n for n in g['nodes'] if n['id'].startswith('/d/qiaoyin/')]
    assert len(ids) == 35 and {n['area'] for n in ids} == {'喬陰縣'}
    assert any(n['id'] == '/d/qiaoyin/inn2' and n.get('layer') == '二樓' for n in ids)
    assert any(e['from'] == '/d/oldpine/highway' and e['to'] == '/d/qiaoyin/ngate' for e in g['edges'])
    assert any(e['from'] == '/d/wutang/three_way' and e['to'] == '/d/qiaoyin/highway' for e in g['edges'])
