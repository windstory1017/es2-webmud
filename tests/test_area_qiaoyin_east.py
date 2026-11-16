from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'
E = MUD / 'd' / 'qiaoyin_east'
OPP = {'north': 'south', 'south': 'north', 'east': 'west', 'west': 'east', 'northeast': 'southwest',
       'southwest': 'northeast', 'northwest': 'southeast', 'southeast': 'northwest', 'enter': 'out', 'out': 'enter',
       'eastdown': 'westup', 'westup': 'eastdown'}
NOSTART = set('﹗！？﹖，﹐。﹒、；﹔：﹕」』）】〉》…')


def read(p):
    return p.read_text(encoding='utf-8')


def exits(path):
    m = re.search(r'set\("exits", \(\[(.*?)\]\)\);', read(path), re.S)
    out = {}
    for d, rel, ab in re.findall(r'"([a-z]+)" : (?:__DIR__"([^"]+)"|"(/[^"]+)")', m.group(1) if m else ''):
        out[d] = rel or ab
    return out


def long_of(path):
    return re.search(r'@LONG\n(.*?)\nLONG', read(path), re.S).group(1)


def wrap(s, width=70, first=4):
    """精確移植 cjk_wrap（房間 look：indent 0、第一行縮 4）。"""
    out, L, prev = '', first, 0
    for ch in s:
        cw = 2 if ord(ch) > 127 else 1
        if ch in '\n\r':
            if prev > 1:
                continue
            ch = ' '
        prev = cw
        if (cw > 1 or ch == ' ') and L + cw > width and ch not in NOSTART:
            out += '\n'
            L = 0
        if ch == ' ' and L == 0:
            continue
        out += ch
        L += cw
    return out.split('\n')


def test_rooms_and_npcs():
    rooms = sorted(p.stem for p in E.glob('*.c'))
    assert len(rooms) == 17
    assert sorted(p.stem for p in (E / 'npc').glob('*.c')) == ['stone_stake', 'tianfong']
    for r in rooms:
        s = read(E / (r + '.c'))
        assert re.search(r'set\("map/area", "(白雲觀|喬陰東郊)"\);', s), r
        for npc in re.findall(r'__DIR__"npc/([a-z_]+)"', s):
            assert (E / 'npc' / (npc + '.c')).exists(), (r, npc)
        for ob in re.findall(r'"(/(?:custom|d|obj)/[^"]+)" : \d', s):
            assert (MUD / (ob.lstrip('/') + '.c')).exists(), (r, ob)
    for f in ['custom/armor/cloth/cyan_taoist_robe.c', 'custom/weapon/sword/peachwood_sword.c',
              'custom/armor/head/copper_hat.c']:
        assert (MUD / f).exists(), f


def test_exits_are_two_way():
    for p in E.glob('*.c'):
        for d, t in exits(p).items():
            if t.startswith('/'):
                dest = MUD / (t.lstrip('/') + '.c')
                back = exits(dest).get(OPP[d])
                assert back and back.endswith(p.stem) or back == '/d/qiaoyin_east/' + p.stem, (p.stem, d, t)
                continue
            if p.stem == 'thicket':      # 雜草叢 east 回岔路口；岔路口是用 pass 草叢進來，沒有對應出口
                continue
            assert exits(E / (t + '.c')).get(OPP[d]) == p.stem, (p.stem, d, t)
    assert exits(MUD / 'd/qiaoyin/egate.c')['northeast'] == '/d/qiaoyin_east/outside'
    assert exits(E / 'outside.c')['southwest'] == '/d/qiaoyin/egate'
    assert exits(E / 'outside.c')['enter'] == 'hall' and exits(E / 'hall.c')['out'] == 'outside'
    assert 'add_action("do_enter", "enter")' in read(E / 'outside.c')
    assert '"enter"' not in read(E / 'fork.c')


def test_mountain_road_slope():
    # 山路往岔路口是下坡（eastdown），岔路口往山路是上坡（westup），兩邊都開（使用者指示，2026-11-16）
    assert exits(E / 'mountain_road.c')['eastdown'] == 'fork'
    assert exits(E / 'fork.c')['westup'] == 'mountain_road'


def test_secret_pass_and_doors():
    s = read(E / 'fork.c')
    assert 'add_action("do_pass", "pass")' in s and 'set("detail/草叢"' in s and 'thicket' in s
    assert '(pass)' in s and '草叢' in long_of(E / 'fork.c')
    assert set(exits(E / 'thicket.c')) == {'east'}
    for a, b in [('cell', 'corridor_w'), ('guest_room', 'sutra_hall')]:
        assert 'create_door("east", "房門", "west", DOOR_CLOSED)' in read(E / (a + '.c'))   # 預設關閉
        assert 'create_door("west", "房門", "east", DOOR_CLOSED)' in read(E / (b + '.c'))
    g = read(E / 'guest_room.c')
    assert 'int do_drink(string arg)' in g and 'stuff = max / 4;' in g and 'supplement_stat("water", stuff)' in g
    assert 'set("detail/茶杯"' in g and '(drink)' in g and '茶杯' in long_of(E / 'guest_room.c')


def test_layout_rules():
    for p in E.glob('*.c'):
        t = long_of(p)
        if p.stem == 'clinic':
            continue
        lines = wrap(t)
        assert 3 <= len(lines) <= 4, p.stem
        assert len(lines[-1].strip()) >= 6, p.stem
        for l in lines[1:]:
            assert l[0] not in NOSTART, (p.stem, l)
        assert not re.search(r'[（(][^（）()\n]{1,14}[）)]', t), p.stem
        assert not re.search(r'\d+ ?級|等級', t), p.stem
    assert '木劍' in long_of(E / 'ground_w.c') and '木槍' not in long_of(E / 'ground_w.c')
    assert '白雲觀' in long_of(E / 'outside.c')


def test_clinic_text_centered():
    t = long_of(E / 'clinic.c')
    assert t.startswith('這裡便是武陀灸堂﹐一陣陣的藥香從裡頭的針房傳了出來。')
    lines = wrap(t)
    assert lines[-2].strip(' 　') == '持針濟世道通當仁不讓' and lines[-1].strip(' 　') == '煉丹救民華陀逃之夭夭'
    for l in lines[-2:]:
        lead = len(l) - len(l.lstrip(' 　'))
        assert sum(2 if c == '　' else 1 for c in l[:lead]) == 25


def test_npcs_placed():
    assert 'lay_ton' in read(E / 'sutra_hall.c') and 'legacy_patrol' in read(MUD / 'custom/legacy_npc/lay_ton.c')
    assert 'wu_dauton' in read(E / 'clinic.c')
    assert 'wu_dauton' not in read(MUD / 'd/snow/square_n.c')
    assert 'legacy_patrol' not in read(MUD / 'custom/legacy_npc/wu_dauton.c').replace('不再巡邏', '')
    assert read(E / 'ground_w.c').count('stone_stake" : 3') == 1 and read(E / 'ground_e.c').count('stone_stake" : 3') == 1
    assert 'tianfong' in read(E / 'hall.c')
    assert '/d/lee/obj/well' in read(E / 'yard.c')


def test_tianfong_quest():
    s = read(E / 'npc/tianfong.c')
    assert 'void relay_say' in s and 'void accept_inquiry' in s
    assert 'improve_skill_exact(SKILL_ID, 100)' in s and '#define MIN_LEVEL  15' in s
    assert '既然你對紫薇心經有興趣' in s
    assert 'query_entry_threshold() { return 100; }' in read(MUD / 'daemon/skill/chivi_sutra.c')
    assert not re.search(r'say[^\n]*\d+ ?級', s)


def test_egate_text():
    t = long_of(MUD / 'd/qiaoyin/egate.c')
    assert '黑風山' in t and '平陽山' not in t and '小徑' in t


def test_web_map():
    g = json.loads(read(ROOT / 'web/world_static_map.json'))
    ids = [n['id'] for n in g['nodes'] if n['id'].startswith('/d/qiaoyin_east/')]
    assert len(ids) == 17
    assert any(e['from'] == '/d/qiaoyin/egate' and e['to'] == '/d/qiaoyin_east/outside' for e in g['edges'])
