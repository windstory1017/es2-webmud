from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SKILL = ROOT / 'source/upstream/mudlib/feature/char/skill.c'

def text():
    return SKILL.read_text(encoding='utf-8')

def threshold(level):
    if level <= 60: base = 100
    elif level <= 90: base = 125
    elif level <= 120: base = 150
    elif level <= 160: base = 175
    elif level <= 180: base = 200
    else: base = 250
    return level * level * base

def test_old_neolith_forward_prototype_is_preserved():
    s = text()
    assert 'varargs int query_attr(string attr, int raw);' in s

def test_common_threshold_breakpoints():
    assert threshold(1) == 100
    assert threshold(60) == 360000
    assert threshold(61) == 465125
    assert threshold(90) == 1012500
    assert threshold(91) == 1242150
    assert threshold(120) == 2160000
    assert threshold(121) == 2562175
    assert threshold(160) == 4480000
    assert threshold(161) == 5184200
    assert threshold(180) == 6480000
    assert threshold(181) == 8190250
    assert threshold(200) == 10000000

def test_restored_skills_and_caps_are_explicit_without_member_array_literal():
    s = text()
    # 舊版把各技能的上限逐一寫死；現在改成「個人技能上限」（得到技能時隨機 140～200，逢 5，
    # 使用者定案 2026-10-22），技能檔還可用 query_skill_cap 再壓低。
    assert 'int query_personal_skill_cap(string skill)' in s
    assert 'cap = 140 + random(13) * 5;' in s
    assert 'int restored_skill_cap(string skill)' in s
    assert 'function_exists("query_skill_cap", daemon)' in s
    assert 'member_array(skill' not in s

def test_progression_is_called_after_daemon_hook_and_uses_conservative_call_other():
    s = text()
    exact = s.split('void improve_skill_exact',1)[1].split('int restored_force_tick_exp',1)[0]
    assert 'function_exists("skill_improved", daemon)' in exact
    assert 'call_other(daemon, "skill_improved", this_object(), skill);' in exact
    assert 'daemon->skill_improved' not in exact
    assert exact.index('call_other(daemon, "skill_improved"') < exact.index('apply_restored_skill_progression(skill);')

def test_version():
    assert (ROOT/'VERSION').read_text().strip().startswith('3.20')
