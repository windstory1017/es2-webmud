from pathlib import Path
import json
import re
import unicodedata

# 紫煙鎮（水嵐縣設計表第四批，docs/AREA_SWAYLAN.md）。
ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'
Z = MUD / 'd' / 'ziyan'
OPP = {'north': 'south', 'south': 'north', 'east': 'west', 'west': 'east', 'northeast': 'southwest',
       'southwest': 'northeast', 'northwest': 'southeast', 'southeast': 'northwest', 'in': 'out', 'out': 'in'}
# 大堂往東可以走出縣衙大門，大門要敲鼓才進得去；茅屋 out 回窄街，進去要 enter 茅屋。
ONE_WAY = {('/d/ziyan/yamen_hall', 'east'), ('/d/ziyan/hut', 'out')}
LEGACY = {'inn'}
NO_LINE_START = '﹗！？﹖，﹐。﹒、；﹔：﹕」』）】〉》…'


def read(p):
    return p.read_text(encoding='utf-8')


def exits(room_id):
    p = MUD / (room_id.lstrip('/') + '.c')
    m = re.search(r'set\("exits", \(\[(.*?)\]\)\);', read(p), re.S)
    if not m:
        return {}
    base = room_id.rsplit('/', 1)[0] + '/'
    return {d: (t if rel == '' else base + t)
            for d, rel, t in re.findall(r'"([a-z]+)" : (__DIR__)?"([^"]+)"', m.group(1))}


def widths(text, width=70, first=4):
    w = [first]
    for ch in text:
        cw = 2 if unicodedata.east_asian_width(ch) in 'WF' else 1
        if w[-1] + cw > width and ch not in NO_LINE_START:
            w.append(0)
        w[-1] += cw
    return w


def test_rooms_and_npcs():
    rooms = sorted(Z.glob('*.c'))
    assert len(rooms) == 22
    for p in rooms:
        s = read(p)
        assert 'set("map/area", "紫煙鎮");' in s, p.stem
        for npc in re.findall(r'__DIR__"npc/([a-z_]+)"', s):
            assert (Z / 'npc' / (npc + '.c')).exists(), (p.stem, npc)
        for f in re.findall(r'"(/[a-z_/]+)" : \d', s):
            assert (MUD / (f.lstrip('/') + '.c')).exists(), (p.stem, f)


def test_exits_are_two_way():
    for p in Z.glob('*.c'):
        rid = '/d/ziyan/' + p.stem
        for d, t in exits(rid).items():
            if (rid, d) in ONE_WAY:
                continue
            assert exits(t).get(OPP[d]) == rid, (rid, d, t)
    assert exits('/d/swaylan/west_gate')['west'] == '/d/ziyan/entrance'


def test_descriptions():
    hint = re.compile(r'[（(][^（）()\n]{1,14}[）)]|[a-z]{3,}')
    for p in Z.glob('*.c'):
        t = re.search(r'@LONG\n(.*?)\nLONG', read(p), re.S).group(1)
        w = widths(t)
        assert max(w) <= 70, (p.stem, w)
        if p.stem not in LEGACY:
            assert len(w) in (3, 4) and w[-1] >= 12, (p.stem, w)
            assert not hint.search(t), p.stem


def test_mechanisms():
    s = read(Z / 'yamen_gate.c')
    assert 'add_action("do_knock", "knock");' in s and 'move("/d/ziyan/yamen_hall")' in s
    assert 'west' not in exits('/d/ziyan/yamen_gate')
    s = read(Z / 'yamen_hall.c')
    assert '"木像" :' in s and '"三足金蟾" :' in s and 'arg != "眼睛"' in s and 'stone_key' in s
    for r in ('yamen_hall', 'inner_hall'):
        assert '"lock" : "ziyan_inner_hall"' in read(Z / (r + '.c')), r
    assert 'set("lock", "ziyan_inner_hall");' in read(Z / 'obj/stone_key.c')
    s = read(Z / 'inner_hall.c')
    assert '誰！來人啊～！' in s and 'called_help' in s and 'yamen_guard' in s and 'constable' in s
    assert 'valid_leave' in read(Z / 'jail.c') and 'lin yu lun' in read(Z / 'jail.c')
    s = read(Z / 'house.c')
    assert '"血手印" :' in s and 'arg != "書櫃"' in s and 'bloody_letter' in s
    assert '上面看起來似字非字、又不像是什麼有意義的符號。' in read(Z / 'obj/bloody_letter.c')
    assert 'arg != "茅屋"' in read(Z / 'narrow_street.c')
    assert 'inherit INN;' in read(Z / 'inn.c') and 'inherit HOCKSHOP;' in read(Z / 'pawnshop.c')


def test_shops():
    for npc in ('waiter', 'herbalist'):
        s = read(Z / 'npc' / (npc + '.c'))
        for f in re.findall(r'"(/[a-z_/]+)" : \d+', s):
            assert (MUD / (f.lstrip('/') + '.c')).exists(), (npc, f)
    assert 'VASE_CAPACITY 5000' in read(Z / 'obj/vase.c')
    assert 'BOTTLE_CAPACITY 1000' in read(Z / 'obj/white_wine.c')
    s = read(Z / 'obj/pig_heart.c')
    assert 'set("food_stuff", 40);' in s and 'set("heal_kee", 5);' in s


def test_web_map():
    g = json.loads(read(ROOT / 'web/world_static_map.json'))
    ids = {n['id'] for n in g['nodes'] if n['id'].startswith('/d/ziyan/')}
    assert ids == {'/d/ziyan/' + p.stem for p in Z.glob('*.c')}


if __name__ == '__main__':
    for name, fn in list(globals().items()):
        if name.startswith('test_') and callable(fn):
            fn()
    print('ok')
