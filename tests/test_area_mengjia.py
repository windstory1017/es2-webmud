from pathlib import Path
import json
import re
import unicodedata

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'
S = MUD / 'd' / 'mengjia'
OPP = {'north': 'south', 'south': 'north', 'east': 'west', 'west': 'east', 'up': 'down', 'down': 'up'}
# 舊資料全文引用（或設計表原文）的房間不受行數限制。
LEGACY = {'herb_shop', 'forest', 'graveyard', 'tomb', 'passage'}
GRID = {'backyard': 'B8', 'manor': 'B10', 'gate': 'B12', 'herb_shop': 'B14', 'inn2': 'D8', 'inn': 'D10',
        'crossroad': 'D12', 'street': 'F12', 'forest': 'F10', 'graveyard': 'F8', 'tomb': 'F6', 'passage': '暗道'}


def read(p):
    return p.read_text(encoding='utf-8')


def exits(name):
    m = re.search(r'set\("exits", \(\[(.*?)\]\)\);', read(S / (name + '.c')), re.S)
    return {d: (t if rel == '' else '/d/mengjia/' + t)
            for d, rel, t in re.findall(r'"([a-z]+)" : (__DIR__)?"([^"]+)"', m.group(1))}


def short_exits(name):
    return {d: t.rsplit('/', 1)[1] if t.startswith('/d/mengjia/') else t for d, t in exits(name).items()}


def long_text(name):
    return re.search(r'@LONG\n(.*?)\nLONG', read(S / (name + '.c')), re.S).group(1)


def test_rooms_and_npcs():
    assert sorted(p.stem for p in S.glob('*.c')) == sorted(GRID)
    for r in GRID:
        s = read(S / (r + '.c'))
        assert 'set("map/area", "孟家莊");' in s
        for npc in re.findall(r'__DIR__"npc/([a-z_]+)"', s):
            assert (S / 'npc' / (npc + '.c')).exists(), (r, npc)
        for o in re.findall(r'__DIR__"obj/([a-z_]+)"', s):
            assert (S / 'obj' / (o + '.c')).exists(), (r, o)
    assert 'set("map/layer", "二樓");' in read(S / 'inn2.c')
    assert 'set("map/layer", "暗道");' in read(S / 'passage.c')
    assert 'set("map/layer", "地下");' in read(S / 'tomb.c')


def test_exits_are_two_way():
    for r in GRID:
        for d, t in exits(r).items():
            if t.startswith('/d/fengcheng/') or r in ('passage', 'tomb'):   # 墓室的 up 是 roll 石像下來的回程
                continue
            assert exits(t.rsplit('/', 1)[1]).get(OPP[d]) == '/d/mengjia/' + r, (r, d, t)


def test_connections_and_special_exits():
    assert short_exits('crossroad')['south'] == '/d/fengcheng/stone_road1'
    assert '"north" : "/d/mengjia/crossroad"' in read(MUD / 'd/fengcheng/stone_road1.c')
    # 暗道：客棧二樓 push 床板 進來；up 回客棧二樓、north 到密林（單向，密林沒有往暗道的出口）
    assert short_exits('passage') == {'up': 'inn2', 'north': 'forest'}
    assert 'add_action("do_push", "push")' in read(S / 'inn2.c') and '"/d/mengjia/passage"' in read(S / 'inn2.c')
    assert 'passage' not in read(S / 'forest.c')
    # 祖墳 roll 石像 進墓室；墓室 up 回祖墳
    assert 'add_action("do_roll", "roll")' in read(S / 'graveyard.c') and '"/d/mengjia/tomb"' in read(S / 'graveyard.c')
    assert short_exits('tomb') == {'up': 'graveyard'}
    assert '幾百年的風吹雨打' in long_text('graveyard') and '的的' not in long_text('graveyard')


def test_inn_herb_and_tiger():
    assert 'inherit INN;' in read(S / 'inn.c')                      # 一樓可存檔
    s2 = read(S / 'inn2.c')
    assert 'inherit ROOM;' in s2 and 'no_fight' not in s2            # 二樓戰區
    h = read(S / 'herb_shop.c')
    assert 'inherit HERB;' in h and '"/d/snow/npc/herbalist"' in h   # 同雪亭小藥鋪
    f = read(S / 'forest.c')
    assert 'ob->query("white_tiger")' in f and 'return ::valid_leave(me, dir);' in f
    assert 'set_beast(8);' in read(S / 'npc' / 'white_tiger.c')


def test_chest_refills_book_each_reset():
    c = read(S / 'obj' / 'chest.c')
    assert 'void reset()' in c and 'refill();' in c and 'manual_of_six_yin_and_eight_trigrams' in c
    assert (MUD / 'obj/books/manual_of_six_yin_and_eight_trigrams.c').exists()
    assert 'obj/chest' in read(S / 'tomb.c')


def test_npc_settings():
    jh = read(MUD / 'custom/legacy_npc/meng_jiang_hong.c')
    assert 'set_level(35);' in jh and 'set_power("A");' in jh
    assert 'set_skill("five-tiger blade", 160);' in jh and 'map_skill("blade", "five-tiger blade");' in jh
    zz = read(S / 'npc' / 'meng_zhao_zhan.c')
    assert 'set_level(35);' in zz and 'set_power("A");' in zz and 'set_skill("five-tiger blade", 160);' in zz
    hx = read(MUD / 'custom/legacy_npc/hua_xing.c')
    assert 'set_level(30);' in hx and 'set_power("A");' in hx and 'set_class("thief");' in hx
    tg = read(S / 'npc' / 'tomb_guard.c')
    assert 'set_level(50);' in tg and 'set_power("A");' in tg and 'exorcist_cassock' in tg
    assert 'set_level(30);' in read(S / 'npc' / 'tian_shen.c')


def test_five_tiger_blade_skill():
    s = read(MUD / 'daemon/skill/five-tiger_blade.c')
    assert s.count('"name": "') == 5 and 'art_usage = "blade";' in s
    assert '#define TRIGGER_PCT 10' in s and '#define DELAY_TICKS 3' in s
    assert 'victim->start_busy(DELAY_TICKS);' in s and '!userp(me) && random(100) < TRIGGER_PCT' in s
    assert '孟家祖傳絕學『五 虎 斷 門』刀風過處退路盡斷﹐$n被封得動彈不得！' in s
    assert 'message_vision("\\n" + HIY + msg + NOR + "\\n\\n", me, victim);' in s    # 整段亮黃、前後空一行
    assert '外人學不得' in s and 'int valid_learn(object me)' in s                 # 玩家不能學
    assert '"five-tiger blade":"五虎斷門刀"' in read(MUD / 'data/chinese.o')


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
    ids = [n for n in g['nodes'] if n['id'].startswith('/d/mengjia/')]
    assert len(ids) == 12 and {n['area'] for n in ids} == {'孟家莊'}
    assert any(e['from'] == '/d/fengcheng/stone_road1' and e['to'] == '/d/mengjia/crossroad' for e in g['edges'])
    pos = [(n['x'], n['y'], n.get('layer')) for n in ids]
    assert len(pos) == len(set(pos))
