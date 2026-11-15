from pathlib import Path
import json
import re
import unicodedata

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'
B = MUD / 'd' / 'baobao'
OPP = {'north': 'south', 'south': 'north', 'east': 'west', 'west': 'east', 'up': 'down', 'down': 'up'}
# 單向路線：歐陽書房 → 兵器庫 → 鑄刀房 → 拳屋 → 鬼境 → 魔境 → 幻境 → 頂樓
ROUTE = [('study', 'west', 'armory'), ('armory', 'up', 'forge'), ('forge', 'east', 'fist_room'),
         ('fist_room', 'up', 'ghost_realm'), ('ghost_realm', 'north', 'magic_realm'),
         ('magic_realm', 'west', 'illusion_realm'), ('illusion_realm', 'up', 'top_floor')]
ROOMS = ['study', 'armory', 'forge', 'fist_room', 'ghost_realm', 'magic_realm', 'illusion_realm', 'top_floor']
LEGACY = {'study', 'forge', 'fist_room', 'ghost_realm', 'magic_realm', 'illusion_realm', 'top_floor'}


def read(p):
    return p.read_text(encoding='utf-8')


def exits(name):
    m = re.search(r'set\("exits", \(\[(.*?)\]\)\);', read(B / (name + '.c')), re.S)
    return {d: (t if rel == '' else '/d/baobao/' + t)
            for d, rel, t in re.findall(r'"([a-z]+)" : (__DIR__)?"([^"]+)"', m.group(1))}


def long_text(name):
    return re.search(r'@LONG\n(.*?)\nLONG', read(B / (name + '.c')), re.S).group(1)


def test_rooms_and_one_way_route():
    assert sorted(p.stem for p in B.glob('*.c')) == sorted(ROOMS)
    for r in ROOMS:
        s = read(B / (r + '.c'))
        assert 'set("map/area", "八寶樓");' in s and 'set("map/mode", "maze");' in s, r
    assert exits('study') == {'south': '/d/fengcheng/inner_yard', 'west': '/d/baobao/armory'}
    for a, d, b in ROUTE:
        assert exits(a).get(d) == '/d/baobao/' + b, (a, d, b)
        if a != 'study':
            assert set(exits(a)) == {d}, a          # 單向：除了往前，沒有別的出口
    assert not re.search(r'set\("exits"', read(B / 'top_floor.c'))    # 頂樓只能跳窗
    assert '"north" : "/d/baobao/study"' in read(MUD / 'd/fengcheng/inner_yard.c')


def test_descriptions():
    assert '石室窄而深長' in long_text('armory') and '木梯通往樓上' in long_text('armory')
    assert '幾張狂草' in long_text('study') and '三十個骷髏頭' in long_text('illusion_realm')
    hint = re.compile(r'[（(][^（）()\n]{1,14}[）)]|[a-z]{3,}')
    for r in ROOMS:
        assert not hint.search(long_text(r)), r
        w = [4]
        for ch in long_text(r):
            cw = 2 if unicodedata.east_asian_width(ch) in 'WF' else 1
            if w[-1] + cw > 70 and ch not in '﹗！？﹖，﹐。﹒、；﹔：﹕」』）】〉》…':
                w.append(0)
            w[-1] += cw
        assert max(w) <= 70, (r, w)
        if r not in LEGACY:
            assert len(w) in (3, 4) and w[-1] >= 12, (r, w)


def test_top_floor_jump():
    s = read(B / 'top_floor.c')
    assert 'add_action("do_jump", "jump");' in s and 'arg != "窗戶"' in s
    assert 'random(300) >= me->query_skill("dodge")' in s       # 摔倒機率 = 1 - dodge/300
    assert 'damage_stat("kee", max / 10)' in s and 'me->unconcious();' in s
    assert '/d/fengcheng/general_gate' in s
    assert '縱身一躍 (jump) 就可以回到將軍府前廣場' in s


def test_npcs():
    L = MUD / 'custom' / 'legacy_npc'
    want = {'o_yan_zhi': ('30', '"B"', 'scholar.myst'), 'si_ma_lei': ('60', '"A"', None),
            'chong_bai_hu': ('50', '"A"', 'fighter.tiger'), 'ma_zhong_shen': ('30', '"B"', None),
            'gong_sun_wei': ('45', '"A"', 'thief.longye'), 'bai_yu': ('50', '"A"', 'taoist.storm'),
            'xun_yuan_jian_yue': ('65', '"S"', 'fighter.snow')}
    for f, (lv, pw, fac) in want.items():
        s = read(L / (f + '.c'))
        assert f'set_level({lv});' in s and f'set_power({pw});' in s, f
        if fac:
            assert f'"custom_faction", "{fac}"' in s, f
    assert 'set("age", 30);' in read(L / 'o_yan_zhi.c') and 'set("age", 280);' in read(L / 'bai_yu.c')
    assert 'set("age", 60);' in read(L / 'xun_yuan_jian_yue.c') and 'set_class("taoist");' in read(L / 'bai_yu.c')
    assert 'legacy_aggressive' not in read(L / 'gong_sun_wei.c').replace('legacy_aggressive", 1', '')
    assert 'set_skill("fonxiaoken", 200);' in read(L / 'ma_zhong_shen.c')
    assert '鬼鬼祟祟地躲在暗處' in read(L / 'gong_sun_wei.c')
    shi = read(B / 'npc' / 'shi_yu.c')
    assert 'set("age", 65);' in shi and 'set_level(30);' in shi and 'set_power("B");' in shi
    for p in ('hundreds_poison_sword', 'padded_armor', 'cloud_boots'):
        assert p in shi
    assert (MUD / 'custom/weapon/sword/hundreds_poison_sword.c').exists()
    assert (MUD / 'custom/armor/armor/padded_armor.c').exists() and (MUD / 'custom/armor/feet/cloud_boots.c').exists()
    objs = read(B / 'ghost_realm.c')
    assert 'obj/skeleton' in objs and 'obj/corpse' in objs and 'npc' not in read(B / 'obj' / 'skeleton.c')
    assert '枯乾的骸骨' in read(B / 'obj' / 'skeleton.c') and '腐爛的男屍' in read(B / 'obj' / 'corpse.c')


def test_web_map():
    g = json.loads(read(ROOT / 'web/world_static_map.json'))
    n = [x for x in g['nodes'] if x['id'].startswith('/d/baobao/')]
    assert len(n) == 1 and n[0]['id'] == '/d/baobao/study' and n[0]['area'] == '八寶樓'
    assert (n[0]['x'], n[0]['y']) == (4, -13)
    yard = [x for x in g['nodes'] if x['id'] == '/d/fengcheng/inner_yard'][0]
    assert (yard['x'], yard['y'] - 1) == (n[0]['x'], n[0]['y'])
    es = {(e['from'], e['direction'], e['to']) for e in g['edges']}
    assert ('/d/fengcheng/inner_yard', 'north', '/d/baobao/study') in es
    assert ('/d/baobao/study', 'south', '/d/fengcheng/inner_yard') in es


if __name__ == '__main__':
    for k, v in list(globals().items()):
        if k.startswith('test_'):
            v()
    print('ok')
