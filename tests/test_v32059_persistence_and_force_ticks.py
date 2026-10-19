from pathlib import Path
import random

ROOT = Path(__file__).resolve().parents[1]

def src(rel):
    return (ROOT / rel).read_text(encoding='utf-8')


def force_tick(seed, con, cps, intel):
    rng = random.Random(seed)
    return (rng.randrange(con) if con > 0 else 0) + (rng.randrange(cps) if cps > 0 else 0) * (intel // 7)


def tiger_growth_delta(old, new):
    """Model only the permanent deltas encoded in tiger_force.c."""
    out = {"gin": 0, "kee": 0, "str": 0, "cor": 0}
    old = max(old, 14)
    for lv in range(old + 1, new + 1):
        if lv >= 161:
            out["gin"] += 1; out["kee"] += 1
        elif lv >= 141:
            out["gin"] += 3; out["kee"] += 4
        else:
            out["gin"] += 2; out["kee"] += 2
        if lv == 140:
            out["cor"] += 2
    return out


def test_shared_force_tick_formula_is_single_source_of_truth():
    s = src('source/upstream/mudlib/feature/char/skill.c')
    assert 'int restored_force_tick_exp()' in s
    assert 'query_attr("int") / 7' in s
    assert '+ (cps > 0 ? random(cps) : 0) * ib' in s
    assert 'void improve_restored_force_tick()' in s
    assert 'improve_skill_exact("force", restored_force_tick_exp());' in s


def test_tiger_and_dragon_force_both_use_shared_basic_force_tick():
    tiger = src('source/upstream/mudlib/daemon/skill/tiger_force.c')
    dragon = src('source/upstream/mudlib/d/snow/skill/dragon_force.c')
    assert 'me->improve_restored_force_tick();' in tiger
    assert 'me->improve_restored_force_tick();' in dragon
    # The old basic-force amount must not survive inside Dragon Force.
    assert 'improve_skill("force", random(me->query_attr("int")/10) + 1)' not in dragon
    # Dragon Force's own gain: int formula + random(spi) (user decision, 2026-10-19).
    assert 'improve_skill("dragon force", random(me->query_attr("con")/10) + 1)' not in dragon
    assert 'COMBAT_D->int_formula_exp(me)' in dragon and 'random(me->query_attr("spi"))' in dragon


def test_force_tick_formula_floor_and_precedence_regression():
    # Formula is random(CON) + random(CPS) * floor(INT/7), not
    # (random(CON)+random(CPS))*floor(INT/7).
    assert force_tick(1, 20, 15, 6) < 20  # INT base zero leaves CON term alive.
    assert force_tick(2, 20, 15, 7) >= 0
    # Deterministic fingerprints catch accidental regrouping later.
    assert [force_tick(32059+i, 20, 17, 21) for i in range(5)] == [59, 16, 46, 4, 21]


def test_skill_and_learned_are_save_object_persistent_not_static():
    skill = src('source/upstream/mudlib/feature/char/skill.c')
    save = src('source/upstream/mudlib/feature/save.c')
    assert 'mapping skills          = ([]);' in skill
    assert 'mapping learned         = ([]);' in skill
    assert 'static mapping learned' not in skill
    assert 'static mapping skills' not in skill
    assert 'save_object (file)' in save and 'restore_object (file, 1)' in save


def test_tiger_growth_markers_are_persistent_dbase_not_temp():
    tiger = src('source/upstream/mudlib/daemon/skill/tiger_force.c')
    assert 'me->set("tiger_force/growth_level", lv);' in tiger
    assert 'me->set("tiger_force/branch_90", 1);' in tiger
    assert 'me->set("tiger_force/cor_bonus_140", 1);' in tiger
    assert 'set_temp("tiger_force/growth_level"' not in tiger
    assert 'set_temp("tiger_force/branch_90"' not in tiger
    assert 'set_temp("tiger_force/cor_bonus_140"' not in tiger


def test_five_reload_cycles_do_not_regrant_permanent_growth_model():
    # Simulates five save/reload points: saved growth marker resumes exactly
    # where it stopped, so already granted levels add zero a second time.
    checkpoints = [20, 100, 140, 160, 181]
    marker = 20
    totals = {"gin": 0, "kee": 0, "str": 0, "cor": 0}
    for level in checkpoints[1:]:
        delta = tiger_growth_delta(marker, level)
        for k, v in delta.items():
            totals[k] += v
        marker = level  # persisted marker after save
        assert tiger_growth_delta(marker, level) == {"gin":0,"kee":0,"str":0,"cor":0}
    # Expected cumulative permanent growth from 21 through 181.
    assert totals == {"gin": 321, "kee": 341, "str": 0, "cor": 2}


def test_growth_boundaries_100_140_141_160_161_181_are_exact():
    assert tiger_growth_delta(99, 100) == {"gin":2,"kee":2,"str":0,"cor":0}
    assert tiger_growth_delta(139, 140) == {"gin":2,"kee":2,"str":0,"cor":2}
    assert tiger_growth_delta(140, 141) == {"gin":3,"kee":4,"str":0,"cor":0}
    assert tiger_growth_delta(159, 160) == {"gin":3,"kee":4,"str":0,"cor":0}
    assert tiger_growth_delta(160, 161) == {"gin":1,"kee":1,"str":0,"cor":0}
    assert tiger_growth_delta(180, 181) == {"gin":1,"kee":1,"str":0,"cor":0}
