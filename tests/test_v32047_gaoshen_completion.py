from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / "source" / "upstream" / "mudlib"


def text(rel):
    return (MUD / rel).read_text(encoding="utf-8")


def test_tiger_steps_has_five_dodge_narratives_and_neutral_dodge_formula():
    # Five dodge narratives from the Han Xiao battle record; shown only when the
    # dodge succeeds (handed to combatd through the attacker's defend_message).
    s = text("daemon/skill/tiger-steps.c")
    assert s.count('"$n') == 5
    assert 'return me->query_skill("dodge")' in s
    assert 'valid_enable(string usage)' in s and 'usage == "dodge"' in s
    assert 'set_temp("defend_message"' in s
    assert 'message_vision' not in s
    assert 'int query_entry_level() { return 10; }' in s
    assert '你已經掌握了狻猊步法。' in s
    assert 'gain_score("martial art", lv * 10)' in s
    assert 'gain_score("martial mastery", (lv - 40) * 10)' in s
    assert 'apply/dodge' not in s
    assert 'random(100)' not in s


def test_tiger_force_user_growth_table_and_powerup_formula():
    s = text("daemon/skill/tiger_force.c")
    assert 'lv >= 161' in s and 'lv >= 141 && lv <= 160' in s
    assert 'advance_stat("gin", lv >= 161 ? 1 : 2 + (lv >= 141 && lv <= 160 ? 1 : 0))' in s
    assert 'advance_stat("kee", lv >= 161 ? 1 : 2 + (lv >= 141 && lv <= 160 ? 2 : 0))' in s
    # 精氣上限不再受「瘋虎功等級 × 12」限制（使用者定案，2026-10-23）。
    assert 'cap = lv * 12;' not in s and 'query_stat_maximum("gin") <= cap' not in s
    assert 'gain_score("martial art", lv * 10)' in s
    # 90 -> 91 branch: combat exp over 100000 jumps to 100 with str+2 con+1.
    assert 'lv == 91' in s and 'score/combat") > 100000' in s
    assert 'query_attr("str", 1) + 2' in s and 'advance_skill("tiger force", 9)' in s
    assert 'lv == 140' in s and 'query_attr("cor", 1) + 2' in s
    assert 'damage_bonus = sk / 4' in s
    assert 'attack_bonus = sk / 3' in s
    assert 'duration = sk * 3 / 2' in s
    assert 'sk < 100' in s
    assert 'consume_stat("kee", 2)' in s and 'damage_stat("kee", 1)' in s
    assert 'consume_stat("gin", 3)' in s and 'damage_stat("gin", 1)' in s


def test_gao_shen_tiger_force_completes_at_twenty_and_grants_root_bonus():
    # 高慎只傳授入門點數；累積到 20 級門檻後下 gain 才練成 20 級並給根骨。
    g = text("d/oldpine/npc/kao_shen.c")
    assert 'me->set_skill(skill, 20)' not in g
    assert 'me->improve_skill_exact(skill,' in g
    s = text("daemon/skill/tiger_force.c")
    assert 'int query_entry_level() { return 15; }' in s
    assert 'int query_entry_threshold() { return 10000; }' in s
    assert 'void skill_completed(object me, string sk)' in s
    assert 'query_attr("con", 1) + 1' in s
    assert 'me->set("tiger_force/growth_level", lv);' in s


def test_tiger_blade_followup_and_moves_remain_present():
    # tiger-blade rewritten on the new martial art engine: eight moves from the
    # Han Xiao battle record; follow-ups need full (100%) force.
    s = text("daemon/skill/tiger-blade.c")
    assert s.count('"action":') == 8
    assert 'me->query_skill("tiger-blade", 1) < 90 || ratio < 100' in s
    assert 'first_follow = strike' in s
    assert 'if( first_follow <= 0' in s
