from pathlib import Path
import json
import re
import unicodedata

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'
S = MUD / 'd' / 'tianshan'
OPP = {'north': 'south', 'south': 'north', 'east': 'west', 'west': 'east', 'up': 'down', 'down': 'up',
       'westdown': 'eastup', 'eastup': 'westdown'}
# 舊資料全文引用的房間不受行數限制（新寫的只有 dirt_road2、dirt_road3、garden）。
NEW = {'dirt_road2', 'dirt_road3', 'garden'}
GRID = {'fog': 'N10', 'spring': 'N8', 'cave1': 'P8', 'cave2': 'R8', 'cave_mouth': 'T8', 'dirt_road1': 'V8',
        'fork': 'X8', 'peach_grove': 'Z8', 'dirt_road2': 'X6', 'dirt_road3': 'X4', 'cabin': 'X2', 'garden': 'X10',
        'hall': 'V2', 'secret_room': 'T2'}


def read(p):
    return p.read_text(encoding='utf-8')


def exits(name):
    m = re.search(r'set\("exits", \(\[(.*?)\]\)\);', read(S / (name + '.c')), re.S)
    return {d: (t if rel == '' else '/d/tianshan/' + t)
            for d, rel, t in re.findall(r'"([a-z]+)" : (__DIR__)?"([^"]+)"', m.group(1))}


def short_exits(name):
    return {d: t.rsplit('/', 1)[1] if t.startswith('/d/tianshan/') else t for d, t in exits(name).items()}


def long_text(name):
    return re.search(r'@LONG\n(.*?)\nLONG', read(S / (name + '.c')), re.S).group(1)


def test_rooms_and_npcs():
    assert sorted(p.stem for p in S.glob('*.c')) == sorted(GRID)
    for r in GRID:
        s = read(S / (r + '.c'))
        assert 'set("map/area", "天山");' in s
        for npc in re.findall(r'__DIR__"npc/([a-z_]+)"', s):
            assert (S / 'npc' / (npc + '.c')).exists(), (r, npc)
    for r, lay in (('cave1', '洞穴'), ('cave2', '洞穴'), ('hall', '內堂'), ('secret_room', '密室')):
        assert 'set("map/layer", "%s");' % lay in read(S / (r + '.c')), r


def test_exits_follow_legacy_record():
    assert short_exits('fog') == {'westdown': '/d/fengcheng/east_gate'}
    assert short_exits('spring') == {'south': 'fog'}
    assert short_exits('cave1') == {'out': 'spring', 'east': 'cave2'}
    assert short_exits('cave2') == {'west': 'cave1'}
    assert short_exits('cave_mouth') == {'west': 'cave2'}
    assert short_exits('dirt_road1') == {'west': 'cave_mouth', 'east': 'fork'}
    assert short_exits('fork') == {'north': 'dirt_road2', 'west': 'dirt_road1', 'south': 'garden', 'east': 'peach_grove'}
    assert short_exits('peach_grove') == {'west': 'fork'}
    assert short_exits('cabin') == {'south': 'dirt_road3'}
    assert short_exits('hall') == {'out': 'cabin'}
    assert short_exits('secret_room') == {'up': 'hall'}
    assert '"eastup" : "/d/tianshan/fog"' in read(MUD / 'd/fengcheng/east_gate.c')


def test_exits_are_two_way():
    for r in GRID:
        for d, t in exits(r).items():
            if t.startswith('/d/fengcheng/') or d not in OPP or r in ('secret_room', 'spring', 'cave_mouth', 'dirt_road1'):
                continue   # 山洞的 out、密室的 up 是指令進去後的回程
            assert exits(t.rsplit('/', 1)[1]).get(OPP[d]) == '/d/tianshan/' + r, (r, d, t)


def test_command_exits():
    assert 'add_action("do_search", "msearch")' in read(S / 'fog.c') and '"山路"' in read(S / 'fog.c')
    assert '"/d/tianshan/spring"' in read(S / 'fog.c')
    assert '"縫隙"' in read(S / 'spring.c') and '"/d/tianshan/cave1"' in read(S / 'spring.c')
    assert '"洞口"' in read(S / 'cave2.c') and '"/d/tianshan/cave_mouth"' in read(S / 'cave2.c')
    assert '"小徑"' in read(S / 'cave_mouth.c') and '"/d/tianshan/dirt_road1"' in read(S / 'cave_mouth.c')
    assert '"靈位"' in read(S / 'hall.c') and '"/d/tianshan/secret_room"' in read(S / 'hall.c')


def test_secret_room_learn_not_yet_open():
    s = read(S / 'secret_room.c')
    assert 'add_action("do_wipe", "wipe")' in s and 'add_action("do_learn", "learn")' in s
    assert 'set_skill' not in s and 'intrinsic-liuh-ken' not in s     # 內八套先做房間，learn 之後再開


def test_npcs():
    q = read(S / 'npc' / 'liuh_qingqing.c')
    assert 'ob->id("gong")' in q and '#define HALL  "/d/tianshan/hall"' in q and 'call_out("greet"' in q
    assert '你好啊，我最近要祭奠我爺爺了。' in q
    assert '/custom/legacy_npc/mountain_god' in read(S / 'peach_grove.c')
    g = read(MUD / 'custom/legacy_npc/mountain_god.c')
    assert 'carry_object("/obj/books/book_of_anatomy");' in g          # 桃仙身上有【脈穴內經】
    assert (MUD / 'obj/books/book_of_anatomy.c').exists()
    assert 'id("gong")' not in read(MUD / 'd/fengcheng/obj/offering.c') and '"gong"' in read(MUD / 'd/fengcheng/obj/offering.c')


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
        if r in NEW:
            assert len(widths) in (3, 4) and widths[-1] >= 12, (r, widths)


def test_web_map():
    g = json.loads(read(ROOT / 'web/world_static_map.json'))
    ids = [n for n in g['nodes'] if n['id'].startswith('/d/tianshan/')]
    assert len(ids) == 14 and {n['area'] for n in ids} == {'天山'}
    assert any(e['from'] == '/d/fengcheng/east_gate' and e['to'] == '/d/tianshan/fog' for e in g['edges'])
    pos = [(n['x'], n['y'], n.get('layer')) for n in ids]
    assert len(pos) == len(set(pos))
