from pathlib import Path
import json

# 羿水擺渡：五堂鎮河邊 ↔ 紫煙鎮渡口（docs/AREA_SWAYLAN.md「船夫擺渡」，使用者指示 2026-10-30）。
ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'


def read(p):
    return (MUD / p).read_text(encoding='utf-8')


def test_ferryman_rules():
    s = read('d/wutang/npc/ferryman.c')
    assert '#define FARE        50' in s and '#define CROSS_TICKS 5' in s
    assert 'string *BANKS = ({ "/d/wutang/riverside", "/d/ziyan/ferry" });' in s
    assert 'ob->value() < FARE' in s and 'crossing = 1;' in s
    for npc, boat in (('ro', 'ferry_boat_ro'), ('huang', 'ferry_boat_huang')):
        s = read('d/wutang/npc/%s.c' % npc)
        assert 'inherit "/d/wutang/npc/ferryman";' in s
        assert 'set("boat_room", "/d/wutang/%s");' % boat in s
        assert '五十文錢' in s and 'set("chat_chance", 15);' in s
    s = read('d/wutang/npc/huang.c')
    assert 'present("ferry' + 'man ro", env)' in s and '++together < 3' in s


def test_no_fight():
    for r in ('d/wutang/riverside', 'd/wutang/ferry', 'd/wutang/ferry_dock', 'd/ziyan/ferry',
              'd/swaylan/ferry', 'd/swaylan/old_ferry', 'd/wutang/ferry_boat_ro', 'd/wutang/ferry_boat_huang'):
        assert 'set("no_fight", 1);' in read(r + '.c'), r


def test_boats_have_no_exits():
    for r in ('ferry_boat_ro', 'ferry_boat_huang'):
        assert 'set("exits"' not in read('d/wutang/%s.c' % r), r


def test_web_map():
    g = json.loads((ROOT / 'web/world_static_map.json').read_text(encoding='utf-8'))
    ids = {n['id'] for n in g['nodes']}
    assert {'/d/wutang/ferry_boat_ro', '/d/wutang/ferry_boat_huang'} <= ids
    # 兩艘船各自一層，地圖上一次只顯示一艘（使用者指示，2026-10-30）。
    layers = {n['id']: n.get('layer') for n in g['nodes'] if n['id'].startswith('/d/wutang/ferry_boat_')}
    assert layers == {'/d/wutang/ferry_boat_ro': '老駱的船', '/d/wutang/ferry_boat_huang': '小黃的船'}


if __name__ == '__main__':
    for name, fn in list(globals().items()):
        if name.startswith('test_') and callable(fn):
            fn()
    print('ok')
