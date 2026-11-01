from pathlib import Path

MUD = Path(__file__).resolve().parents[1] / 'source' / 'upstream' / 'mudlib'


def read(p):
    return (MUD / p).read_text(encoding='utf-8')


def test_spawn_and_power():
    # 2026-10-30 起在冷梅莊正廳出生（d/swaylan/lunmay/hall.c）。
    assert '"/d/wutang/npc/may_yin_fong" : 1,' in read('d/swaylan/lunmay/hall.c')
    s = read('d/wutang/npc/may_yin_fong.c')
    for line in ['set_name("梅影風", ({ "may yin fong", "may", "fong" }));', 'set_race("human");',
                 'set_class("fighter");', 'set_level(60);', 'set("age", 55);', 'set_power("S");']:
        assert line in s


def test_skills_and_mapping():
    s = read('d/wutang/npc/may_yin_fong.c')
    want = {'lunmay': 180, 'sword': 180, 'force': 190, 'hainmay force': 200, 'secondhand sword': 180,
            'secondhand dagger': 180, 'advance_lunmay': 180, 'parry': 160, 'dodge': 150, 'mayin': 160,
            'unarmed': 140, 'seven': 160}
    for sk, lv in want.items():
        assert f'set_skill("{sk}", {lv});' in s, sk
    for sk in ['lunmay', 'advance_lunmay', 'hainmay_force', 'mayin', 'seven']:
        assert (MUD / 'daemon/skill' / (sk + '.c')).exists(), sk


def test_equipment():
    s = read('d/wutang/npc/may_yin_fong.c')
    for f in ['custom/armor/armor/silver_platemail_of_frost', 'custom/armor/feet/boots_of_flying_cloud',
              'custom/armor/cloth/white_robe', 'custom/armor/finger/white_ring',
              'custom/armor/waist/white_girth', 'custom/armor/waist/silver_girth',
              'custom/weapon/sword/sword_of_frost_edge', 'custom/weapon/dagger/dagger_of_frost_edge']:
        assert ('/' + f) in s, f
        assert (MUD / (f + '.c')).exists(), f
    assert s.count('boots_of_flying_cloud') == 1


def test_stays_in_hall():
    # 搬回冷梅莊後不走動（使用者指示，2026-10-30）。
    s = read('d/wutang/npc/may_yin_fong.c')
    assert 'wander' not in s and 'call_out' not in s


def test_apprentice_rules():
    s = read('d/wutang/npc/may_yin_fong.c')
    assert '閣下的志向相當明確，又何必來糾纏老朽呢？' in s
    assert '閣下似乎不適合修習本門武功。' in s
    assert '({ "ashura", "malik", "rainner" })' in s
    assert 'me->set("title", "冷梅莊弟子");' in s
    assert '#define FACTION         "fighter.lunmay"' in s
    assert 'me->skill_threshold(skill, 1)' in s
