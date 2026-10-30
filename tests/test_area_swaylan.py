from pathlib import Path
import json
import re
import unicodedata

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'
S = MUD / 'd' / 'swaylan'
L = S / 'lunmay'
OPP = {'north': 'south', 'south': 'north', 'east': 'west', 'west': 'east', 'northeast': 'southwest',
       'southwest': 'northeast', 'northwest': 'southeast', 'southeast': 'northwest', 'up': 'down', 'down': 'up'}
# 舊資料全文引用的房間不受長度限制。
LEGACY = {'inn', 'inn2', 'north_street2', 'pawnshop', 'front_yard', 'hall'}


def all_rooms():
    return sorted(S.glob('*.c')) + sorted(L.glob('*.c'))


def rid(p):
    return '/' + p.relative_to(MUD).with_suffix('').as_posix()


def read(p):
    return p.read_text(encoding='utf-8')


def exits(room_id):
    # 傳回 {方向: 目的地房間代號}，__DIR__ 換成絕對路徑。
    p = MUD / (room_id.lstrip('/') + '.c')
    m = re.search(r'set\("exits", \(\[(.*?)\]\)\);', read(p), re.S)
    if not m:
        return {}
    base = room_id.rsplit('/', 1)[0] + '/'
    return {d: (t if rel == '' else base + t)
            for d, rel, t in re.findall(r'"([a-z]+)" : (__DIR__)?"([^"]+)"', m.group(1))}


def long_text(p):
    return re.search(r'@LONG\n(.*?)\nLONG', read(p), re.S).group(1)


def test_rooms_and_npcs():
    assert len(list(S.glob('*.c'))) == 37
    assert len(list(L.glob('*.c'))) == 12
    for p in all_rooms():
        s = read(p)
        area = '冷梅莊' if p.parent == L else '(水嵐縣|黃家鏢局)'
        assert re.search(r'set\("map/area", "' + area + r'"\);', s), p.stem
        for npc in re.findall(r'__DIR__"npc/([a-z_0-9]+)"', s):
            assert (p.parent / 'npc' / (npc + '.c')).exists(), (p.stem, npc)
        for f in re.findall(r'"(/[a-z_/0-9]+)" : \d', s):
            assert (MUD / (f.lstrip('/') + '.c')).exists(), (p.stem, f)


def test_exits_are_two_way():
    for p in all_rooms():
        for d, t in exits(rid(p)).items():
            assert exits(t).get(OPP[d]) == rid(p), (rid(p), d, t)


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
    # 自寫敘述三～四行：首行空四格、每行 70 寬（中文算 2），最後一行至少六個字；
    # 標點不可剛好掛在行尾凸出去（使用者指示，2026-10-30）。
    # 舊資料不受行數限制，但同樣要對齊（使用者指示：對齊為主，可微調舊文）。
    for p in all_rooms():
        widths = wrapped_widths(long_text(p))
        assert max(widths) <= 70, (p.stem, widths)
        if p.stem not in LEGACY:
            assert len(widths) in (3, 4) and widths[-1] >= 12, (p.stem, widths)


def test_no_command_hints():
    hint = re.compile(r'[（(][^（）()\n]{1,14}[）)]|[a-z]{3,}')
    for p in all_rooms():
        assert not hint.search(long_text(p)), p.stem


def test_special_rooms():
    assert 'inherit INN;' in read(S / 'inn.c')
    assert 'inherit BANK;' in read(S / 'bank.c')
    assert 'inherit HOCKSHOP;' in read(S / 'pawnshop.c')
    assert 'inherit ROOM;' in read(S / 'chien_store.c')   # 錢記商號不是銀行
    assert 'set("map/layer", "二樓");' in read(S / 'inn2.c')
    assert '__DIR__"npc/pawn_guard" : 2,' in read(S / 'pawnshop.c')   # 守衛兩名（使用者指示，2026-10-30）
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
    assert ids == {rid(p) for p in all_rooms()}


def test_lunmay_manor():
    # 梅影風搬回冷梅莊正廳、不走動、不再傳授冷梅劍法（使用者指示，2026-10-30）。
    assert '"/d/wutang/npc/may_yin_fong" : 1,' in read(L / 'hall.c')
    assert 'may_yin_fong' not in read(MUD / 'd/wutang/crossroad.c')
    s = read(MUD / 'd/wutang/npc/may_yin_fong.c')
    assert 'wander' not in s
    assert '冷梅劍法的要訣需要自行體會，等你能夠一劍七環時就算有所小成了。' in s
    assert '"north" : __DIR__"lunmay/gate",' in read(S / 'crossroad.c')
    for r in ('side_hall', 'bedroom', 'corridor_w', 'corridor_e'):
        assert 'create_door(' in read(L / (r + '.c')), r
    # 前院穿環：冷梅莊弟子、手上有劍、已會就不用練；耗精 10、忙碌 1；36 次學成。
    s = read(L / 'front_yard.c')
    for part in ('add_action("do_practise", "practise");', 'arg != "lunmay"',
                 '"fighter.lunmay"', 'query_temp("weapon/sword")',
                 '你已經練成冷梅劍法，不用再在這裡練習了。',
                 '#define RINGS_DONE  36', '#define GIN_COST    10', 'me->start_busy(1);',
                 'rings = 1 + to_int(sqrt(to_float(count)));', 'apply_gain_progression'):
        assert part in s, part
    # tell_room 的排除對象一定要是陣列；直接傳人物會讓遊戲當機（2026-10-30 實機測到）。
    assert ', me);' not in s and s.count('({ me }));') == 2


if __name__ == '__main__':
    for name, fn in list(globals().items()):
        if name.startswith('test_') and callable(fn):
            fn()
    print('ok')
