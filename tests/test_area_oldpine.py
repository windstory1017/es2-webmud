from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'
O = MUD / 'd' / 'oldpine'
OPP = {'north': 'south', 'south': 'north', 'east': 'west', 'west': 'east', 'northeast': 'southwest',
       'southwest': 'northeast', 'northwest': 'southeast', 'southeast': 'northwest', 'up': 'down', 'down': 'up'}


def read(p):
    return p.read_text(encoding='utf-8')


def exits(name):
    m = re.search(r'set\("exits", \(\[(.*?)\]\)\);', read(O / (name + '.c')), re.S)
    return dict(re.findall(r'"([a-z]+)" : (?:__DIR__)?"([^"]+)"', m.group(1))) if m else {}


def test_rooms_npcs_and_areas():
    rooms = sorted(p.stem for p in O.glob('*.c'))
    assert len(rooms) == 50   # 46 間房間，蘆葦叢迷宮多出 reeds2～reeds5 四間
    assert len(list((O / 'npc').glob('*.c'))) == 18
    areas = {}
    for r in rooms:
        s = read(O / (r + '.c'))
        a = re.search(r'set\("map/area", "([^"]+)"\);', s).group(1)
        areas[a] = areas.get(a, 0) + 1
        for npc in re.findall(r'__DIR__"npc/([a-z_]+)"', s):
            assert (O / 'npc' / (npc + '.c')).exists(), (r, npc)
    assert areas == {'老松林': 22, '迷霧森林': 28}


def test_exits_are_two_way():
    # 一般出口都要能走回來；蘆葦叢（迷宮）與出口房、指令出口除外。
    skip = {'reeds', 'reeds2', 'reeds3', 'reeds4', 'reeds5', 'reeds_exit', 'wood3', 'ledge'}
    for p in O.glob('*.c'):
        for d, t in exits(p.stem).items():
            if d not in OPP or t.startswith('/') or p.stem in skip:
                continue
            assert exits(t).get(OPP[d]) == p.stem, (p.stem, d, t)


def test_entrance_from_snow_south_gate():
    assert '"southeast" : "/d/oldpine/entrance"' in read(MUD / 'd/snow/sgate.c')
    assert exits('entrance')['northwest'] == '/d/snow/sgate'


def test_reed_maze_rooms():
    # 五間一模一樣的蘆葦叢用一般出口串起來：n、n、e、w、n 走到出口房，走錯回到 reeds。
    seq = ['north', 'north', 'east', 'west', 'north']
    rooms = ['reeds', 'reeds2', 'reeds3', 'reeds4', 'reeds5']
    longs = {re.search(r'@LONG\n(.*?)\nLONG', read(O / (r + '.c')), re.S).group(1) for r in rooms}
    assert len(longs) == 1
    for k, r in enumerate(rooms):
        ex = exits(r)
        assert set(ex) == {'north', 'south', 'east', 'west'}
        assert ex[seq[k]] == (rooms[k + 1] if k + 1 < len(rooms) else 'reeds_exit')
        for d in set(ex) - {seq[k]}:
            assert ex[d] == ('wood3' if (k == 0 and d == 'east') else 'reeds'), (r, d)
        assert 'set("map/layer", "蘆葦叢");' in read(O / (r + '.c'))
    assert exits('reeds_exit') == {'east': 'wood3'}
    assert exits('wood3')['west'] == 'reeds'


def test_command_exits_and_inn():
    assert 'add_action("do_cave", "cave")' in read(O / 'grass2.c')
    assert exits('inn')['enter'] == 'kitchen' and 'inherit INN;' in read(O / 'inn.c')
    assert exits('entrance')['south'] == 'clearing_w' and exits('clearing_w')['north'] == 'entrance'
    assert exits('clearing_w')['southeast'] == 'path0' and exits('path0') == {'northwest': 'clearing_w', 'east': 'path1'}
    assert exits('path1')['west'] == 'path0'
    assert exits('clearing_n') == {'south': 'grass1'} and exits('grass1')['north'] == 'clearing_n'
    assert 'north' not in exits('forest_n3')
    assert 'add_action("do_climb", "climb")' in read(O / 'cave_deep.c')
    assert exits('ledge') == {'down': 'cave_deep'}
    assert exits('kitchen') == {'out': 'inn'}


def test_bandit_bounty_and_aggression():
    # 土匪、徐彪看到玩家就下殺手；土匪嘍囉不主動攻擊。
    for f, n, aggro in [('bandit', 15, True), ('bandit_minion', 5, False), ('xue_biao', 110, True)]:
        s = read(O / 'npc' / (f + '.c'))
        assert 'set("bounty", ([ "military service": %d ]));' % n in s
        assert ('kill_ob(ob);' in s) == aggro, f
    # 懸賞只由 CHAR_D->make_corpse() 發一次。
    die = read(MUD / 'std/char/npc.c').split('void\ndie()', 1)[1].split('\n}', 1)[0]
    assert 'bounty' not in die and 'gain_score' not in die


def test_beast_strengths():
    want = {'deer': 3, 'rat': 1, 'squirrel': 1, 'wolf': 4, 'wild_bear': 7, 'little_bear': 4, 'big_bear': 5}
    for f, n in want.items():
        assert 'set_beast(%d);' % n in read(O / 'npc' / (f + '.c')), f
    assert 'set_beast(4);' in read(MUD / 'd/wutang/npc/boar.c')
    assert 'set_beast(5);' in read(MUD / 'd/wutang/npc/big_boar.c')
    assert 'set_beast(2);' in read(MUD / 'd/wutang/npc/sheep.c')
    npc = read(MUD / 'std/char/npc.c')
    assert 'RACE_D("beast")->set_strength(this_object(), beast_strength);' in npc
    race = read(MUD / 'daemon/race/beast.c')
    assert '"gin":  20, "kee":  30' in race and '"gin": 150, "kee": 250' in race


def test_web_map_has_both_areas():
    g = json.loads(read(ROOT / 'web/world_static_map.json'))
    ids = [n for n in g['nodes'] if n['id'].startswith('/d/oldpine/')]
    assert len(ids) == 50
    assert {n['area'] for n in ids} == {'老松林', '迷霧森林'}
    assert any(e['from'] == '/d/snow/sgate' and e['to'] == '/d/oldpine/entrance' for e in g['edges'])


def test_room_descriptions_have_no_command_hints():
    # 房間敘述不用括號提示指令或方向（例如「（climb 石頭）」「(暗巷)」），由敘述本身暗示。
    hint = re.compile(r'[（(][^（）()\n]{1,14}[）)]')
    for base in (O, MUD / 'd' / 'wutang'):
        for p in base.glob('*.c'):
            m = re.search(r'@LONG\n(.*?)\nLONG', read(p), re.S)
            assert not (m and hint.search(m.group(1))), (p.name, hint.findall(m.group(1)))
