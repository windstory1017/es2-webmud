from pathlib import Path
import json
import re
import unicodedata

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'
S = MUD / 'd' / 'fengcheng'
OPP = {'north': 'south', 'south': 'north', 'east': 'west', 'west': 'east', 'up': 'down', 'down': 'up',
       'northup': 'southdown', 'southdown': 'northup'}
# 舊資料全文引用的房間不受行數限制。
LEGACY = {'cliff', 'mountain_road', 'li_house', 'crossroad', 'south_street', 'inn'}
GRID = {'qingyun_temple': 'J10', 'cliff': 'L10', 'inner_yard': 'H12', 'mountain_road': 'J12', 'east_gate': 'L12',
        'stone_road1': 'D14', 'li_house': 'F14', 'general_gate': 'H14', 'stone_road2': 'L14', 'crossroad': 'D16',
        'street1': 'F16', 'street2': 'H16', 'east_street1': 'J16', 'east_street2': 'L16', 'south_street': 'D18',
        'inn': 'F18', 'inn2': 'F18 二樓', 'alley': 'L18', 'tower': 'B20', 'gate': 'D20'}


def read(p):
    return p.read_text(encoding='utf-8')


def exits(name):
    m = re.search(r'set\("exits", \(\[(.*?)\]\)\);', read(S / (name + '.c')), re.S)
    return {d: (t if rel == '' else '/d/fengcheng/' + t)
            for d, rel, t in re.findall(r'"([a-z]+)" : (__DIR__)?"([^"]+)"', m.group(1))}


def short_exits(name):
    return {d: t.rsplit('/', 1)[1] if t.startswith('/d/fengcheng/') else t for d, t in exits(name).items()}


def long_text(name):
    return re.search(r'@LONG\n(.*?)\nLONG', read(S / (name + '.c')), re.S).group(1)


def test_rooms_and_npcs():
    assert sorted(p.stem for p in S.glob('*.c')) == sorted(GRID)
    for r in GRID:
        s = read(S / (r + '.c'))
        assert 'set("map/area", "風城");' in s
        for npc in re.findall(r'__DIR__"npc/([a-z_]+)"', s):
            assert (S / 'npc' / (npc + '.c')).exists(), (r, npc)
    assert 'set("map/layer", "二樓");' in read(S / 'inn2.c')
    assert 'set("map/layer", "城樓");' in read(S / 'tower.c')
    assert len(list((S / 'npc').glob('*.c'))) == 7


def test_exits_are_two_way():
    for r in GRID:
        for d, t in exits(r).items():
            if t.startswith(('/d/shuiyue/', '/d/mengjia/', '/d/tianshan/')) or d == 'northup':
                continue
            assert exits(t.rsplit('/', 1)[1]).get(OPP[d]) == '/d/fengcheng/' + r, (r, d, t)


def test_connections():
    assert short_exits('gate')['south'] == '/d/shuiyue/stone_road'
    assert read(MUD / 'd/shuiyue/stone_road.c').count('"north" : "/d/fengcheng/gate"') == 1
    # 東城門往北上坡到孤崖、孤崖往南下坡；往東上坡接天山霧海（天山 2026-11-14 做好後開通）。
    assert short_exits('east_gate') == {'west': 'mountain_road', 'south': 'stone_road2', 'northup': 'cliff',
                                        'eastup': '/d/tianshan/fog'}
    assert short_exits('cliff') == {'southdown': 'east_gate'}
    # 石板路往北接孟家莊十字路口（孟家莊 2026-11-14 做好後開通）。
    assert short_exits('stone_road1') == {'north': '/d/mengjia/crossroad', 'south': 'crossroad', 'east': 'li_house'}
    assert short_exits('inn')['up'] == 'inn2' and short_exits('inn2') == {'down': 'inn'}


def test_inn_and_no_fight():
    assert 'inherit INN;' in read(S / 'inn.c')            # 一樓：非戰區、可存檔
    s2 = read(S / 'inn2.c')
    assert 'inherit ROOM;' in s2 and 'no_fight' not in s2   # 二樓：戰區
    assert 'set("no_fight", 1);' in read(S / 'general_gate.c')


def test_gate_guard_blocks_until_riddle_solved():
    g = read(S / 'general_gate.c')
    assert 'dir == "north" && !me->query("quest/fengcheng_riddle")' in g
    assert 'guard->warn(me);' in g and 'return ::valid_leave(me, dir);' in g
    c = read(S / 'npc' / 'child.c')
    assert 'who->set("quest/fengcheng_riddle", 1);' in c
    # 小孩：玩家進來就問要不要猜；用 say 或正面表情同意；答案也用 say（不用 ask）。
    assert 'void relay_say(object who, string msg)' in c and 'void relay_emote(object who, string verb)' in c
    assert 'accept_inquiry' not in c and '要不要來猜個謎語啊' in c and '將軍府裡藏了八個鬼喔' in c
    assert 'message_vision("$N把$n攔了下來。\\n", this_object(), who);' in read(S / 'npc' / 'gate_guard.c')
    assert len(re.findall(r'^    \(\{ "', c, re.M)) == 10     # 舊資料的十題字謎
    assert '({ "弓上一枝箭，箭邊一把弓", "引" })' in c


def test_seller_and_items():
    s = read(S / 'npc' / 'seller.c')
    assert '"/d/fengcheng/obj/offering" : 10' in s
    assert '"/custom/item/scroll/blank" : 2000' in s and '"/custom/item/dragon_skin_bag" : 5' in s
    assert 'set("value", 500);' in read(S / 'obj' / 'offering.c')
    assert 'set("value", 200000);' in read(MUD / 'custom/item/dragon_skin_bag.c')   # 20 兩黃金
    assert (MUD / 'custom/item/scroll/blank.c').exists() and (MUD / 'custom/item/dragon_skin_bag.c').exists()


def test_npcs_and_purple_helmet():
    h = read(S / 'npc' / 'hua_zi_yi.c')
    assert 'set_level(60);' in h and 'set_power("S");' in h and 'numinous_sword' in h and 'purple_helmet' in h
    helm = read(MUD / 'custom/armor/head/purple_helmet.c')
    assert '"wis": 1,' in helm and '"spi": 1,' in helm and '"armor": 25,' in helm
    assert 'set_weight(2000);' in helm and 'set("value", 12000);' in helm
    f = read(S / 'npc' / 'fortune_teller.c')
    assert '發展已經到了極限' in f and '還可以繼續發展' in f
    chen = read(S / 'npc' / 'chen_xue_liang.c')
    assert 'set_power("A");' in chen
    # 強化之術：ask chen about 強化之術，條件（法術 60、等級 10、兵刃精熟 50000）夠了就三句話後傳授。
    assert 'who->query_skill("magic", 1) < 60' in chen and 'who->query_level() < 10' in chen
    assert 'who->query_score("weapon mastery") < 50000' in chen
    assert chen.count('call_out("say_to"') == 2 and 'call_out("teach", 3 * TICK, who);' in chen
    assert 'who->set_skill(SKILL_ID, 1);' in chen and '#define SKILL_ID  "enchance"' in chen
    assert (MUD / 'daemon/skill/enchance.c').exists()
    assert '/obj/area/garrison' in read(S / 'tower.c') and 'si_ma_wei' in read(S / 'tower.c')


NO_LINE_START = '﹗！？﹖，﹐。﹒、；﹔：﹕」』）】〉》…'


def wrapped_widths(text, width=70, first=4):
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


def test_web_map():
    g = json.loads(read(ROOT / 'web/world_static_map.json'))
    ids = [n for n in g['nodes'] if n['id'].startswith('/d/fengcheng/')]
    assert len(ids) == 20 and {n['area'] for n in ids} == {'風城'}
    assert any(e['from'] == '/d/shuiyue/stone_road' and e['to'] == '/d/fengcheng/gate' for e in g['edges'])
    pos = [(n['x'], n['y'], n.get('layer')) for n in ids]
    assert len(pos) == len(set(pos))
