from pathlib import Path
import json
import re
import unicodedata

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'
S = MUD / 'd' / 'swaylan'
OPP = {'north': 'south', 'south': 'north', 'east': 'west', 'west': 'east', 'northeast': 'southwest',
       'southwest': 'northeast', 'northwest': 'southeast', 'southeast': 'northwest', 'up': 'down', 'down': 'up'}
# 舊資料全文引用的房間不受長度限制。
LEGACY = {'inn', 'inn2', 'north_street2', 'pawnshop'}


def read(p):
    return p.read_text(encoding='utf-8')


def exits(name):
    s = read(S / (name + '.c'))
    m = re.search(r'set\("exits", \(\[(.*?)\]\)\);', s, re.S)
    if not m:
        return {}
    return dict(re.findall(r'"([a-z]+)" : (?:__DIR__)?"([^"]+)"', m.group(1)))


def long_text(name):
    return re.search(r'@LONG\n(.*?)\nLONG', read(S / (name + '.c')), re.S).group(1)


def test_rooms_and_npcs():
    rooms = sorted(p.stem for p in S.glob('*.c'))
    assert len(rooms) == 37
    for r in rooms:
        s = read(S / (r + '.c'))
        assert re.search(r'set\("map/area", "(水嵐縣|黃家鏢局)"\);', s), r
        for npc in re.findall(r'__DIR__"npc/([a-z_]+)"', s):
            assert (S / 'npc' / (npc + '.c')).exists(), (r, npc)
        for f in re.findall(r'"(/[a-z_/]+)" : \d', s):
            assert (MUD / (f.lstrip('/') + '.c')).exists(), (r, f)


def test_exits_are_two_way():
    for p in S.glob('*.c'):
        for d, t in exits(p.stem).items():
            assert exits(t).get(OPP[d]) == p.stem, (p.stem, d, t)


def test_description_length():
    # 自寫敘述三～四行：首行空四格、每行 70 寬（中文算 2），最後一行至少六個字。
    for p in S.glob('*.c'):
        if p.stem in LEGACY:
            continue
        n = sum(2 if unicodedata.east_asian_width(c) in 'WF' else 1 for c in long_text(p.stem)) // 2
        assert 74 <= n <= 103 or 109 <= n <= 138, (p.stem, n)


def test_no_command_hints():
    hint = re.compile(r'[（(][^（）()\n]{1,14}[）)]|[a-z]{3,}')
    for p in S.glob('*.c'):
        assert not hint.search(long_text(p.stem)), p.stem


def test_special_rooms():
    assert 'inherit INN;' in read(S / 'inn.c')
    assert 'inherit BANK;' in read(S / 'bank.c')
    assert 'inherit HOCKSHOP;' in read(S / 'pawnshop.c')
    assert 'inherit ROOM;' in read(S / 'chien_store.c')   # 錢記商號不是銀行
    assert 'set("map/layer", "二樓");' in read(S / 'inn2.c')
    for r in ('ferry', 'bank', 'inn2', 'pawnshop'):
        assert 'set("no_fight", 1);' in read(S / (r + '.c')), r
    s = read(S / 'north_gate.c')
    assert 'arg != "破洞"' in s and 'move("/d/swaylan/old_ferry")' in s
    assert '"/custom/legacy_npc/huang_tien_ba" : 1' in read(S / 'escort_hall.c')
    assert '"/custom/legacy_npc/kao_fon" : 1' in read(S / 'escort_side.c')


def test_shops():
    for npc in ('waiter', 'richman', 'smith'):
        s = read(S / 'npc' / (npc + '.c'))
        for f in re.findall(r'"(/[a-z_/]+)" : \d+', s):
            assert (MUD / (f.lstrip('/') + '.c')).exists(), (npc, f)
    assert '"/obj/alchemist/heat_firewood" : 10000' in read(S / 'npc/richman.c')
    assert 'SKIN_CAPACITY 3000' in read(S / 'obj/chrysanthemum_skin.c')
    assert 'BOTTLE_CAPACITY 1000' in read(S / 'obj/white_wine.c')


def test_web_map():
    g = json.loads(read(ROOT / 'web/world_static_map.json'))
    ids = {n['id'] for n in g['nodes'] if n['id'].startswith('/d/swaylan/')}
    assert ids == {'/d/swaylan/' + p.stem for p in S.glob('*.c')}


if __name__ == '__main__':
    for name, fn in list(globals().items()):
        if name.startswith('test_') and callable(fn):
            fn()
    print('ok')
