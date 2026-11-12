from pathlib import Path

# 雪吟莊武功（使用者定案，2026-11-05；docs/martial_arts/雪吟莊_武功.md）。
ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'


def read(p):
    return (MUD / p).read_text(encoding='utf-8')


def test_tidy_force():
    s = read('daemon/skill/tidy_force.c')
    assert 'art_id    = "tidy force";' in s and 'art_entry = 5;' in s
    assert 'me->query_level() < 5' in s and 'is_faction(me, FACTION)' in s
    assert 'me->advance_stat("gin", 2);' in s and 'me->advance_stat("kee", random(2));' in s
    assert 'me->consume_stat("gin", 2);' in s and 'me->consume_stat("kee", 2);' in s
    assert 'me->damage_stat("gin", 1);' in s and 'me->damage_stat("kee", 1);' in s
    assert 'cost = 10 + sk / 10;' in s and 'me->supplement_stat("gin", sk * 3 / 2);' in s
    assert 'me->start_busy(1);' in s


def test_celestial_force():
    s = read('daemon/skill/celestial_force.c')
    assert 'art_entry = 20;' in s and 'me->query_level() < 10' in s
    assert 'me->query_personal_skill_cap("tidy force")' in s
    assert 'me->advance_stat("gin", 1 + random(3));' in s and 'me->advance_stat("kee", 3);' in s
    assert 'me->advance_stat("sen", random(3));' in s
    assert '120: ({ 20, 1 }),' in s and '140: ({ 40, 2 }),' in s and '160: ({ 60, 4 }),' in s
    assert 'me->query("celestial_force/bonus_" + at)' in s and 'me->set_attr("con", me->query_attr("con", 1) + bonus[1]);' in s
    # heal：扣 sk/20 神，回 sk/2+根骨 的氣與格子
    assert 'n = sk / 2 + me->query_attr("con");' in s and 'me->heal_stat("kee", n);' in s
    # roar：只打交手中的對手，暈 2、傷害 sk/5+根骨、自回 sk/2、自身停頓 1
    assert 'foreach( ob in me->query_enemy() )' in s and 'ob->start_busy(2);' in s
    assert 'dmg = sk / 5 + me->query_attr("con");' in s and 'me->supplement_stat("kee", sk / 2);' in s
    assert 'me->start_busy(1);' in s and 'me->start_busy(2);' not in s
    # ghost：警覺 sk/2、攻勢 sk/4、陰陽眼、sk/5 回合
    assert 'aware = sk / 2;' in s and 'intimidate = sk / 4;' in s and 'duration = sk / 5;' in s
    assert 'me->add_temp("apply/vision_of_ghost", 1);' in s


def test_deep_blade():
    s = read('daemon/skill/deep_blade.c')
    assert s.count('"action":') == 5
    for name, color in (('快招二十五', 'HIY'), ('狠招三十六', 'HIR'), ('怪招四十九', 'HIM'),
                        ('險招六十四', 'HIC'), ('奇招八十一', 'HIG')):
        assert '「" %s "%s" NOR "」' % (color, name) in s, name
    assert '#define SPECIAL_LEVEL   60' in s and '#define SPECIAL_CHANCE  66' in s
    assert 'n = damage * (100 + a * b) / 100;' in s
    assert '忽然歪歪斜斜地砍出一刀﹐竟逼得$n避無可避！' in s


def test_six_chaos_sword():
    s = read('daemon/skill/six-chaos-sword.c')
    assert s.count('"action":') == 6
    assert '$N身形一轉﹐一招「厲鬼纏身」﹐$w斬向$n的後心' in s
    assert 'return usage == "sword" || usage == "parry";' in s and 'string parry_message' in s


def test_celestial_palm():
    s = read('daemon/skill/celestial_palm.c')
    assert s.count('"action":') == 6 and 'art_usage = "unarmed";' in s
    assert 'me->query_skill("celestial force", 1) < 180' in s
    assert 'move_chance(me, 10, 30, 20)' in s and 'move_chance(me, 10, 20, 25)' in s
    assert 'opponent->start_busy(2);' in s and 'me->supplement_stat("kee", sk / 6);' in s
    assert 'if( act != "cele zarn" )' in s and 'me->consume_stat("kee", 100);' in s
    assert 'amount = sk * 2 / 5;' in s and 'duration = sk / 5;' in s
    for k in ('apply/armor', 'apply/intimidate', 'apply/wittiness'):
        assert 'me->add_temp("%s", amount);' % k in s


def test_npc_table_and_master():
    t = read('daemon/misc/npc_skill.c')
    assert '"skill": "deep blade", "map": "blade", "level": 1' in t
    assert '"skill": "celestial palm"' not in t   # 雪吟莊弟子不會天邪神掌（2026-11-06）
    y = read('custom/legacy_npc/yu_tieshan.c')
    assert 'int accept_apprentice(object me)' in y and 'me->set("custom_faction", FACTION);' in y
    assert 'daemon->valid_learn(me)' in y
    for f in ('d/snowkeep/npc/apprentice.c', 'd/snowkeep/npc/blademan.c'):
        assert 'set("custom_faction", "fighter.snow");' in read(f)


def test_help_files():
    for f, head in (('tidy_force.txt', '◎ 凌潮心法（tidy force）'), ('celestial_force.txt', '◎ 天邪神功（celestial force）'),
                    ('deep_blade.txt', '◎ 深空刀法（deep blade）'), ('six-chaos-sword.txt', '◎ 六陰追魂劍法（six-chaos-sword）'),
                    ('celestial_palm.txt', '◎ 天邪神掌（celestial palm）')):
        assert read('docs/help/' + f).startswith(head), f
    assert read('docs/help/fighter.snow.txt').startswith('○ 雪吟莊資料')
    assert (ROOT / 'docs/martial_arts/雪吟莊_武功.md').exists()


if __name__ == '__main__':
    for name, fn in list(globals().items()):
        if name.startswith('test_') and callable(fn):
            fn()
    print('ok')
