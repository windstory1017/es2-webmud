from pathlib import Path
import json
import re
import unicodedata

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'
S = MUD / 'd' / 'shuiyue'
OPP = {'north': 'south', 'south': 'north', 'east': 'west', 'west': 'east', 'northeast': 'southwest',
       'southwest': 'northeast', 'northwest': 'southeast', 'southeast': 'northwest', 'up': 'down', 'down': 'up',
       'northup': 'southdown', 'southdown': 'northup'}
# 舊資料全文引用的房間不受行數限制。
LEGACY = {'waterfall', 'cave', 'hunter_hut', 'sky_tree', 'tree_top', 'inn'}
GRID = {'stone_road': 'D22', 'waterfall': 'F20', 'cave': 'H20', 'stone_house': 'J20', 'woods': 'F22',
        'hunter_hut': 'H22', 'mud_road': 'D24', 'sky_tree': 'F24', 'tree_top': 'H24', 'tree_hollow': 'J24',
        'village1': 'D26', 'crossroad': 'D28', 'chief_house': 'B28', 'inn': 'F28', 'village2': 'D30',
        'mountain_path1': 'D32', 'mountain_path2': 'D34'}


def read(p):
    return p.read_text(encoding='utf-8')


def exits(name):
    p = S / (name + '.c')
    m = re.search(r'set\("exits", \(\[(.*?)\]\)\);', read(p), re.S)
    return {d: (t if rel == '' else '/d/shuiyue/' + t)
            for d, rel, t in re.findall(r'"([a-z]+)" : (__DIR__)?"([^"]+)"', m.group(1))}


def long_text(name):
    return re.search(r'@LONG\n(.*?)\nLONG', read(S / (name + '.c')), re.S).group(1)


def short_exits(name):
    return {d: t.rsplit('/', 1)[1] if t.startswith('/d/shuiyue/') else t for d, t in exits(name).items()}


def test_rooms_and_npcs():
    rooms = sorted(p.stem for p in S.glob('*.c'))
    assert rooms == sorted(GRID)
    for r in rooms:
        s = read(S / (r + '.c'))
        assert 'set("map/area", "水月村");' in s
        for npc in re.findall(r'__DIR__"npc/([a-z_]+)"', s):
            assert (S / 'npc' / (npc + '.c')).exists(), (r, npc)
        for o in re.findall(r'__DIR__"obj/([a-z_]+)"', s):
            assert (S / 'obj' / (o + '.c')).exists(), (r, o)
    assert len(list((S / 'npc').glob('*.c'))) == 5
    for r in ('cave', 'stone_house'):
        assert 'set("map/layer", "洞穴");' in read(S / (r + '.c'))
    for r in ('tree_top', 'tree_hollow'):
        assert 'set("map/layer", "樹上");' in read(S / (r + '.c'))


def test_exits_are_two_way():
    for r in GRID:
        for d, t in exits(r).items():
            if t.startswith(('/d/snow/', '/d/fengcheng/')) or d in ('out', 'down'):
                continue   # out、down 是指令出入的回程，對面沒有對應的一般出口
            assert exits(t.rsplit('/', 1)[1]).get(OPP[d]) == '/d/shuiyue/' + r, (r, d, t)


def test_connection_to_snow_and_grid():
    assert short_exits('mountain_path2')['south'] == '/d/snow/npath2'
    assert '"north" : "/d/shuiyue/mountain_path2"' in read(MUD / 'd/snow/npath2.c')
    # D22 沒有往東、F22 沒有往西的出口（使用者指示）；D22 往北接風城城門口。
    assert short_exits('stone_road') == {'north': '/d/fengcheng/gate', 'south': 'mud_road'}
    assert short_exits('woods') == {'north': 'waterfall', 'south': 'sky_tree', 'east': 'hunter_hut'}
    assert short_exits('village2')['southdown'] == 'mountain_path1'
    assert short_exits('mountain_path1')['northup'] == 'village2'


def test_command_exits_and_doors():
    assert 'add_action("do_enter", "enter")' in read(S / 'waterfall.c')
    assert '"/d/shuiyue/cave"' in read(S / 'waterfall.c')
    assert '"縫隙"' in read(S / 'cave.c') and '"/d/shuiyue/stone_house"' in read(S / 'cave.c')
    assert short_exits('cave') == {'out': 'waterfall'} and short_exits('stone_house') == {'out': 'cave'}
    s = read(S / 'sky_tree.c')
    assert 'add_action("do_enter", "climb")' in s and '"卯天樹"' in s and '"丱天樹"' in s
    assert short_exits('tree_top') == {'down': 'sky_tree'} and short_exits('tree_hollow') == {'out': 'tree_top'}
    assert 'create_door("east", "木門", "west", DOOR_CLOSED);' in read(S / 'woods.c')
    assert 'create_door("west", "木門", "east", DOOR_CLOSED);' in read(S / 'hunter_hut.c')


NO_LINE_START = '﹗！？﹖，﹐。﹒、；﹔：﹕」』）】〉》…'


def wrapped_widths(text, width=70, first=4):
    # 照 adm/simul_efun/utf8.c 的 cjk_wrap()：收尾標點不放行首，會掛在上一行尾端。
    widths = [first]
    for ch in text:
        w = 2 if unicodedata.east_asian_width(ch) in 'WF' else 1
        if widths[-1] + w > width and ch not in NO_LINE_START:
            widths.append(0)
        widths[-1] += w
    return widths


def test_description_length():
    for r in GRID:
        widths = wrapped_widths(long_text(r))
        assert max(widths) <= 70, (r, widths)
        if r not in LEGACY:
            assert len(widths) in (3, 4) and widths[-1] >= 12, (r, widths)


def test_no_command_hints_in_long():
    hint = re.compile(r'[（(][^（）()\n]{1,14}[）)]|[a-z]{3,}')
    for r in GRID:
        assert not hint.search(long_text(r)), r


def test_inn_shop_and_tea_pot():
    s = read(S / 'inn.c')
    assert 'set("no_fight", 1);' in s and 'inherit INN' not in s
    assert 'valid_startroom' not in s
    k = read(S / 'npc' / 'innkeeper.c')
    assert '"/obj/food/manto" : 50' in k and '"/d/shuiyue/obj/big_manto" : 50' in k
    m = read(S / 'obj' / 'big_manto.c')
    assert 'set("food_stuff", 200);' in m and 'set("value", 20);' in m
    t = read(S / 'obj' / 'tea_pot.c')
    assert '"tea pot"' in t and '#define POT_VOLUME 100000' in t and 'set("no_get", 1);' in t
    assert 'obj/tea_pot' in read(S / 'crossroad.c')


def test_npcs():
    assert 'set_beast(4);' in read(S / 'npc' / 'leopard.c')
    assert '"/d/oldpine/npc/squirrel"' in read(S / 'tree_top.c')
    assert 'set_beast(1);' in read(MUD / 'd/oldpine/npc/squirrel.c')
    wf = read(S / 'npc' / 'wei_feng.c')
    assert 'set_level(60);' in wf and 'set_power("S");' in wf and 'au_han_sword' in wf
    assert 'custom/weapon/sword/colosus_sword' in wf   # 古劍醒塵被拿走時改拿巨神劍
    ys = read(S / 'npc' / 'yu_tian_shan.c')
    assert 'set_level(40);' in ys and 'set_power("B");' in ys and 'set_class("fighter");' in ys


def test_web_map():
    g = json.loads(read(ROOT / 'web/world_static_map.json'))
    ids = [n for n in g['nodes'] if n['id'].startswith('/d/shuiyue/')]
    assert len(ids) == 17 and {n['area'] for n in ids} == {'水月村'}
    assert any(e['from'] == '/d/snow/npath2' and e['to'] == '/d/shuiyue/mountain_path2' for e in g['edges'])
    # 水月村自己的座標不重疊（各區域各自一套座標，和別區重疊是既有做法）。
    pos = [(n['x'], n['y'], n.get('layer')) for n in ids]
    assert len(pos) == len(set(pos))
