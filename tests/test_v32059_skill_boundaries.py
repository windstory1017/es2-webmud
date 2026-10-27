from pathlib import Path
import random

ROOT = Path(__file__).resolve().parents[1]

def src(rel):
    return (ROOT / rel).read_text(encoding='utf-8')

def common_threshold(level):
    if level <= 60: base = 100
    elif level <= 90: base = 125
    elif level <= 120: base = 150
    elif level <= 160: base = 175
    elif level <= 180: base = 200
    else: base = 250
    return level * level * base

def d_value(player, mob):
    return max(2, min(10, mob-player))

def test_exact_common_threshold_examples_and_breakpoints():
    assert common_threshold(1) == 100
    assert common_threshold(60) == 360_000
    assert common_threshold(61) == 465_125
    assert common_threshold(90) == 1_012_500
    assert common_threshold(91) == 1_242_150
    assert common_threshold(120) == 2_160_000
    assert common_threshold(121) == 2_562_175
    assert common_threshold(160) == 4_480_000
    assert common_threshold(161) == 5_184_200
    assert common_threshold(180) == 6_480_000
    assert common_threshold(181) == 8_190_250
    assert common_threshold(200) == 10_000_000
    assert common_threshold(61)-common_threshold(60) == 105_125

def test_d_boundaries_low_equal_plus_one_two_ten_and_over():
    cases = [(20,1,2),(20,19,2),(20,20,2),(20,21,2),(20,22,2),
             (20,25,5),(20,30,10),(20,99,10)]
    assert [(a,b,d_value(a,b)) for a,b,_ in cases] == [(a,b,x) for a,b,x in cases]

def test_formula_ranges_and_floor_int_base():
    random.seed(32059)
    for intel in [0,1,6,7,13,14,21,35]:
        ib = intel // 7
        d = 10
        vals = [(random.randrange(d)+1)*(random.randrange(d)+1)*ib for _ in range(500)]
        assert min(vals) >= ib
        assert max(vals) <= 100*ib
        if intel < 7:
            assert set(vals) == {0}

def test_every_skill_caps_at_200_and_levels_one_per_gain():
    s=src('source/upstream/mudlib/feature/char/skill.c')
    cap=s.split('int restored_skill_cap(string skill)',1)[1].split('}',1)[0]
    assert 'query_personal_skill_cap(skill)' in cap
    # 個人技能上限（使用者定案，2026-10-22）：玩家隨機 140～200（逢 5），NPC 仍是 200。
    own=s.split('int query_personal_skill_cap(string skill)\n{',1)[1].split('\n}\n',1)[0]
    assert 'if( !userp(this_object()) ) return 200;' in own
    assert 'cap = 140 + random(13) * 5;' in own
    assert 'return 120;' not in s and 'return 140;' not in s
    prog=s.split('void apply_gain_progression(mapping gained)',1)[1].split('\n}\n',1)[0]
    assert 'while' not in prog          # 一次 gain 最多一級
    assert 'skill_next_level(skill)' in prog
    npc=s.split('void apply_restored_skill_progression(string skill)\n{',1)[1].split('\n}\n',1)[0]
    assert 'if( userp(this_object()) ) return;' in npc
    g=src('source/upstream/mudlib/cmds/usr/gain.c')
    assert 'me->apply_gain_progression(skill_g);' in g

def test_tiger_force_completes_at_level15_with_own_threshold():
    t=src('source/upstream/mudlib/daemon/skill/tiger_force.c')
    assert 'int query_entry_level() { return 15; }' in t
    assert 'int query_entry_threshold() { return 10000; }' in t

def test_teacher_only_seeds_up_to_level_one_floor():
    g=src('source/upstream/mudlib/d/oldpine/npc/kao_shen.c')
    assert 'me->query_learn(skill) >= me->skill_threshold(skill, cap)' in g
    assert 'me->skill_threshold(skill, cap) - me->query_learn(skill)' in g
    assert 'me->set_skill(skill, 1)' not in g

def test_trigger_wiring_is_success_only_not_attempt_only():
    c=src('source/upstream/mudlib/adm/daemons/combatd.c')
    f=src('source/upstream/mudlib/feature/char/combat.c')
    assert 'if( damage > 0 ) restored_hit_gain(me, victim, skill, weapon);' in c
    assert 'if( random(100) > chance ) {' in f
    dodge_block=f.split('if( random(100) > chance ) {',1)[1].split('return 0;',1)[0]
    assert 'restored_dodge_gain' in dodge_block
    # parry: full gain after a block (incl. the floor-chance block); a failed parry
    # gives the reduced 1/4 gain (failed = 1). 使用者定案，2026-10-17。
    parry_block=f.split('private int parry_attack(int strength, object from)\n{',1)[1]
    fail_branch=parry_block.split('if( block <= strength ) {',1)[1].split('full = 0;',1)[0]
    assert 'restored_parry_gain(this_object(), attacker, 1);' in fail_branch
    success_part=parry_block.split('full = 0;',1)[1]
    assert 'COMBAT_D->restored_parry_gain(this_object(), attacker);' in success_part
    assert 'varargs void restored_parry_gain(object me, object opponent, int failed)' in c

def test_no_legacy_level_multiplier_in_exact_gain():
    s=src('source/upstream/mudlib/feature/char/skill.c')
    exact=s.split('void improve_skill_exact',1)[1].split('// improve_skill()',1)[0]
    # 2026-10-10 使用者定案：所有技能點數都吃人物等級加成。
    assert 'amount += amount * random(this_object()->query_level() / 8);' in exact
    assert 'skill_gain[skill]' in exact

def advance_model(level, learned, cap=200):
    while level < cap and learned >= common_threshold(level + 1):
        level += 1
    return level

def test_single_exact_boundary_multi_level_jump_and_caps():
    # One point below does not level; exact threshold does.
    assert advance_model(59, common_threshold(60)-1) == 59
    assert advance_model(59, common_threshold(60)) == 60
    # A large gain may legitimately cross several cumulative thresholds.
    assert advance_model(58, common_threshold(63)) == 63
    # The 200 cap stops advancement even with absurd learned totals.
    assert advance_model(199, 99_999_999) == 200
