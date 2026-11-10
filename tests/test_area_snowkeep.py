from pathlib import Path
import json
import re

# 雪吟莊（d/snowkeep）；依使用者提供的雪吟莊設計表（2026-11-06），見 docs/AREA_SNOWKEEP.md。
ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'
K = MUD / 'd' / 'snowkeep'
OPP = {'north': 'south', 'south': 'north', 'east': 'west', 'west': 'east', 'up': 'down', 'down': 'up'}
NEW_ROOMS = ['entrance', 'yard_w', 'yard_e', 'path', 'pavilion', 'garden_s', 'side_n', 'side_s',
             'dungeon_w', 'dungeon', 'dungeon_e']
OLD_ROOMS = ['garden_n', 'hall', 'inner_hall', 'east_room', 'court']


def read(p):
    return p.read_text(encoding='utf-8')


def exits(path):
    m = re.search(r'set\("exits", \(\[(.*?)\]\)\);', read(path), re.S)
    out = {}
    for d, rel, ab in re.findall(r'"([a-z]+)" : (?:__DIR__"([^"]+)"|"(/[^"]+)")', m.group(1) if m else ''):
        out[d] = rel or ab
    return out


def long_text(path):
    return re.search(r'@LONG\n(.*?)\nLONG', read(path), re.S).group(1)


def test_rooms_and_npcs():
    rooms = sorted(p.stem for p in K.glob('*.c'))
    assert rooms == sorted(NEW_ROOMS + OLD_ROOMS)
    assert sorted(p.stem for p in (K / 'npc').glob('*.c')) == ['apprentice', 'blademan', 'jailer', 'wang_hsian']
    for r in rooms:
        s = read(K / (r + '.c'))
        assert 'set("map/area", "雪吟莊");' in s, r
        for npc in re.findall(r'__DIR__"npc/([a-z_]+)"', s):
            assert (K / 'npc' / (npc + '.c')).exists(), (r, npc)
        for ob in re.findall(r'"(/(?:custom|d|obj)/[^"]+)" : \d', s):
            assert (MUD / (ob.lstrip('/') + '.c')).exists(), (r, ob)
    for r in ('dungeon_w', 'dungeon', 'dungeon_e'):
        assert 'set("map/layer", "地下");' in read(K / (r + '.c')), r


def test_exits_are_two_way():
    for p in K.glob('*.c'):
        for d, t in exits(p).items():
            if t.startswith('/'):
                assert t == '/d/snow/ebridge'
                continue
            if p.stem == 'dungeon_w' and d == 'up':
                continue   # 涼亭往下的石階是機關打開的
            assert exits(K / (t + '.c')).get(OPP[d]) == p.stem, (p.stem, d, t)
    assert '"east" : "/d/snowkeep/entrance",' in read(MUD / 'd/snow/ebridge.c')


def test_descriptions():
    for r in NEW_ROOMS:
        t = long_text(K / (r + '.c'))
        assert 74 <= len(t) <= 138, (r, len(t))
        assert '(' not in t and '（' not in t, r
    assert '一條鵝卵石鋪成的小路通像南邊的亭子' in long_text(K / 'garden_n.c')
    assert '換成了一幅京城才子張于辰的字畫' in long_text(K / 'hall.c')
    assert '「六賢」的座位' in long_text(K / 'inner_hall.c')
    assert '地上整齊地鋪著灰石磚。' in long_text(K / 'court.c')


def test_pavilion_mechanism():
    s = read(K / 'pavilion.c')
    assert '#define OPEN_TICKS  450' in s and '#define HURT_FIRST  10' in s and '#define HURT_MORE   5' in s
    assert 'add_action("do_press", "press");' in s and 'set("no_clean_up", 1);' in s
    assert 'set("exits/down", __DIR__"dungeon_w");' in s and 'call_out("close_stairs", OPEN_TICKS * TICK);' in s
    assert 'me->damage_stat("kee", hurt);' in s and 'me->delete_temp("snowkeep/bagua_step");' in s
    assert 'set("detail/八卦桌",' in s


def test_dungeon_door_and_key():
    s = read(K / 'dungeon.c')
    assert 'present("jailer", this_object())' in s and 'present("brass key", me)' not in s
    # 牢門改用標準門：關著時出口不顯示，用鑰匙 unlock、open（2026-11-06）
    assert '"status" : DOOR_CLOSED | DOOR_LOCKED, "lock" : LOCK_ID' in s and 'void reset()' in s
    e = read(K / 'dungeon_e.c')
    assert 'create_door("west",' in e and 'varargs int open_door' in e and 'replace_program' not in e
    assert 'set("lock", "snowkeep_dungeon");' in read(K / 'obj/brass_key.c')
    assert '"/d/snowkeep/obj/brass_key"' in read(MUD / 'custom/legacy_npc/yu_tieshan.c')
    assert 'set_name("黃銅鑰匙", ({ "brass key", "key" }));' in read(K / 'obj/brass_key.c')
    h = read(MUD / 'custom/armor/armor/heart_plate.c')
    assert '"armor": 10,' in h and '"wittiness": 10,' in h


def test_yu_tieshan_combat():
    s = read(MUD / 'custom/legacy_npc/yu_tieshan.c')
    assert 'map_skill("force", "celestial force");' in s
    assert 'celestial palm' not in s
    assert '#define PILL_MAX        4' in s and '#define PILL_RATIO      10' in s
    assert 'exert_function("roar");' in s and 'message_vision(HIG "\\n$N從懷中摸出把丹藥一口吞了下去' in s


def test_web_map():
    g = json.loads(read(ROOT / 'web/world_static_map.json'))
    nodes = [n for n in g['nodes'] if n['id'].startswith('/d/snowkeep/')]
    assert len(nodes) == 16 and {n['area'] for n in nodes} == {'雪吟莊'}
    assert len({(n['x'], n['y']) for n in nodes}) == 16
    assert {n['id'].split('/')[-1] for n in nodes if n.get('layer') == '地下'} == {'dungeon_w', 'dungeon', 'dungeon_e'}
    assert any(e['from'] == '/d/snow/ebridge' and e['to'] == '/d/snowkeep/entrance' for e in g['edges'])
    assert any(e['from'] == '/d/snowkeep/entrance' and e['to'] == '/d/snow/ebridge' for e in g['edges'])
    assert "'/d/snowkeep/dungeon_w': ('/d/snowkeep/pavilion', 0, -1, -1)," in read(ROOT / 'tools/build_codex.py')


def test_yu_lan_jian_yi_combat():
    s = read(MUD / 'custom/legacy_npc/yu_lan_jian_yi.c')
    assert 'map_skill("force", "celestial force");' in s and 'map_skill("unarmed", "celestial palm");' in s
    assert '#define SURGE_MAX       2' in s and '#define SURGE_RATIO     10' in s and '#define SURGE_DAMAGE    50' in s
    assert 'perform_action("celestial palm", "cele zarn");' in s and 'exert_function("roar");' in s
    assert 'foreach( string what in ({ "gin", "kee", "sen", "HP" }) )' in s


def test_guard_no_longer_blocks():
    s = read(MUD / 'd/snow/npc/guard.c')
    assert 'do_go' not in s and 'seteuid(getuid());' in s


if __name__ == '__main__':
    for name, fn in list(globals().items()):
        if name.startswith('test_') and callable(fn):
            fn()
    print('ok')
