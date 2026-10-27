"""康平村（/d/kangping）與百花村（/d/baihua）：依使用者設計表（2026-10-20）。"""
from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'
K = MUD / 'd' / 'kangping'
B = MUD / 'd' / 'baihua'
OPP = {'north': 'south', 'south': 'north', 'east': 'west', 'west': 'east', 'northeast': 'southwest',
       'southwest': 'northeast', 'northwest': 'southeast', 'southeast': 'northwest'}


def read(p):
    return p.read_text(encoding='utf-8')


def exits(path):
    m = re.search(r'set\("exits", \(\[(.*?)\]\)\);', read(path), re.S)
    out = {}
    for d, rel, ab in re.findall(r'"([a-z]+)" : (?:__DIR__"([^"]+)"|"(/[^"]+)")', m.group(1) if m else ''):
        out[d] = rel or ab
    return out


def test_rooms_npcs_and_objects_exist():
    for d, area, n_rooms, n_npcs in ((K, '康平村', 7, 2), (B, '百花村', 15, 12)):
        rooms = sorted(d.glob('*.c'))
        assert len(rooms) == n_rooms
        assert len(list((d / 'npc').glob('*.c'))) == n_npcs
        for p in rooms:
            s = read(p)
            assert f'set("map/area", "{area}");' in s, p
            for npc in re.findall(r'__DIR__"npc/([a-z_]+)"', s):
                assert (d / 'npc' / (npc + '.c')).exists(), (p, npc)
            for ob in re.findall(r'"(/(?:custom|d|obj)/[^"]+)" : \d', s):
                assert (MUD / (ob.lstrip('/') + '.c')).exists(), (p, ob)
    for p in list((K / 'npc').glob('*.c')) + list((B / 'npc').glob('*.c')):
        for ob in re.findall(r'carry_object\("(/[^"]+)"\)', read(p)):
            assert (MUD / (ob.lstrip('/') + '.c')).exists(), (p, ob)


def test_exits_two_way_and_links():
    for d in (K, B):
        for p in d.glob('*.c'):
            for dirn, t in exits(p).items():
                if t.startswith('/') or dirn == 'eastup' or (p.stem, dirn) == ('trail', 'northwest'):
                    continue
                assert exits(d / (t + '.c')).get(OPP[dirn]) == p.stem, (p.stem, dirn, t)
    assert exits(MUD / 'd/oldpine/grass1.c')['east'] == '/d/kangping/forest_exit'
    assert exits(K / 'forest_exit.c')['west'] == '/d/oldpine/grass1'
    assert exits(MUD / 'd/oldpine/grass2.c')['southeast'] == '/d/baihua/hillside'
    assert exits(B / 'hillside.c') == {'northwest': '/d/oldpine/grass2'}
    # 特殊通道：山腰上搜草叢到荒野小徑、荒野小徑往西北回山腰上；百花崖爬峭壁上懸崖平台、平台 eastup 回百花崖
    assert 'add_action("do_search", "search")' in read(B / 'hillside.c') and '__DIR__"trail"' in read(B / 'hillside.c')
    assert exits(B / 'trail.c')['northwest'] == 'hillside'
    assert 'add_action("do_climb", "climb")' in read(B / 'cliff.c') and '__DIR__"platform"' in read(B / 'cliff.c')
    assert exits(B / 'platform.c') == {'eastup': 'cliff'}
    # 谷口不開往三煙谷的出口（使用者決定）
    assert 'north' not in exits(K / 'valley_mouth.c')


def test_inns_and_vendors():
    for p in (K / 'inn.c', B / 'inn.c'):
        s = read(p)
        assert 'inherit INN;' in s and 'set("no_fight", 1);' in s and 'set("valid_startroom", 1);' in s
    for p in (K / 'npc/innkeeper.c', B / 'npc/innkeeper.c'):
        s = read(p)
        assert '"/d/snow/npc/obj/roast_chicken" : 30' in s
        assert '"/obj/food/dumpling" : 20' in s and '"/obj/food/pork" : 10' in s


def test_special_npcs_and_weapons():
    assert 'set("training_dummy", 1);' in read(B / 'npc/stone_lion.c')
    assert '"/obj/area/obj/shortsword")->wield();' in read(B / 'npc/traveller.c')
    assert 'set("custom_faction", "alchemist.longtu");' in read(B / 'npc/wang_yean.c')
    assert 'set("title"' not in read(B / 'npc/wang_yean.c')
    assert 'set("custom_faction", "taoist.storm");' in read(B / 'npc/nan_kong_hen.c')
    s = read(MUD / 'custom/weapon/staff/sinia_staff.c')
    assert 'init_damage(3, 12, 135, 0, "twohanded staff");' in s and '"armor": 5' in s and '"defense": 25' in s
    assert 'set("value", 10000);' in s
    s = read(MUD / 'custom/weapon/staff/shepherd_cane.c')
    assert 'set("wield_as", "staff");' in s and 'set("value", 1000);' in s and '"cane"' in s


def test_web_map_nodes():
    g = json.loads(read(ROOT / 'web/world_static_map.json'))
    ids = {n['id'] for n in g['nodes']}
    for d, area in ((K, 'kangping'), (B, 'baihua')):
        for p in d.glob('*.c'):
            assert f'/d/{area}/{p.stem}' in ids
