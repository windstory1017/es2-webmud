from pathlib import Path
import re

MUD = Path(__file__).resolve().parents[1] / 'source' / 'upstream' / 'mudlib'
W = MUD / 'd' / 'wutang'
OPP = {'north': 'south', 'south': 'north', 'east': 'west', 'west': 'east', 'northeast': 'southwest',
       'southwest': 'northeast', 'northwest': 'southeast', 'southeast': 'northwest', 'up': 'down', 'down': 'up',
       'in': 'out', 'out': 'in', 'northdown': 'southup', 'southup': 'northdown'}


def read(p):
    return p.read_text(encoding='utf-8')


def exits(name):
    s = read(W / (name + '.c'))
    m = re.search(r'set\("exits", \(\[(.*?)\]\)\);', s, re.S)
    if not m:
        return {}
    out = {}
    for d, t in re.findall(r'"([a-z]+)" : (?:__DIR__)?"([^"]+)"', m.group(1)):
        out[d] = t
    return out


def test_rooms_and_npcs():
    # 2026-10-30 加了兩間渡船（地圖區域「羿水」）與船夫共用檔 npc/ferryman.c。
    boats = {'ferry_boat_ro', 'ferry_boat_huang'}
    rooms = sorted(p.stem for p in W.glob('*.c'))
    assert len(rooms) == 58
    assert len(list((W / 'npc').glob('*.c'))) == 33
    for r in rooms:
        s = read(W / (r + '.c'))
        assert ('set("map/area", "羿水");' if r in boats else 'set("map/area", "五堂鎮");') in s, r
        for npc in re.findall(r'__DIR__"npc/([a-z_]+)"', s):
            assert (W / 'npc' / (npc + '.c')).exists(), (r, npc)


def test_exits_are_two_way():
    # 指令出入（enter 暗巷／小巷子 等）不在 exits 裡，其餘一般出口都要能走回來。
    one_way = {('temple_road_s', 'dark_alley_e'), ('yan_gate', 'temple_road_n'), ('temple_yard', 'temple')}
    for p in W.glob('*.c'):
        for d, t in exits(p.stem).items():
            if t.startswith('/'):
                continue
            back = exits(t).get(OPP[d])
            assert back == p.stem or (p.stem, t) in one_way or (t, p.stem) in one_way, (p.stem, d, t, back)


def test_special_moves():
    assert 'add_action("do_enter", "enter")' in read(W / 'temple_road_n.c')
    assert 'add_action("do_pass", "pass")' in read(W / 'gravel_road_n.c')
    assert 'add_action("do_swim", "swim")' in read(W / 'boat.c')
    assert '"northdown" : __DIR__"ferry_dock"' in read(W / 'ferry.c')
    assert '"south" : "/d/wutang/entrance"' in read(MUD / 'custom/zhenwu/room/gate.c')


def test_beast_and_town_god():
    s = read(MUD / 'daemon/race/beast.c')
    assert 'int valid_wield(object me, object ob, string skill) { return 0; }' in s
    assert '"beast"' not in read(MUD / 'adm/daemons/logind.c')
    s = read(W / 'npc/boar.c')
    assert 'random(100) < 20' in s and 'kill_ob(ob)' in s
    s = read(W / 'npc/town_god.c')
    assert 'inherit "/custom/ghost/std_ghost";' in s and 'CHAR_D->make_living(me);' in s


def test_items_and_shops():
    assert 'set("value", 200);' in read(MUD / 'obj/area/obj/cloth.c')
    s = read(W / 'obj/blue_cloth.c')
    assert 'set("value", 300);' in s and '"armor": 4' in s
    assert 'set("base_value", 30);' in read(W / 'obj/pie.c')
    assert '"/d/wutang/obj/pie" : 20' in read(W / 'npc/seller.c')
    for f in ('/custom/armor/finger/han_jade_ring', '/custom/armor/finger/white_jade_ring', '/d/lee/obj/yellow_jade_ring'):
        assert f in read(W / 'npc/jeweller.c')
        assert (MUD / (f.lstrip('/') + '.c')).exists()
