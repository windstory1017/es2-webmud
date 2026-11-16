from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
SKILL=(ROOT/'source/upstream/mudlib/feature/char/skill.c').read_text(encoding='utf-8')
COMBAT=(ROOT/'source/upstream/mudlib/feature/char/combat.c').read_text(encoding='utf-8')
COMBATD=(ROOT/'source/upstream/mudlib/adm/daemons/combatd.c').read_text(encoding='utf-8')
APP=(ROOT/'web/app.js').read_text(encoding='utf-8')

def test_skill_daemon_hook_uses_neolith_native_function_exists():
    assert 'daemon != DAEMON_D' not in SKILL
    assert 'function_exists("skill_improved", daemon)' in SKILL
    assert 'varargs void improve_skill_exact(string skill, int amount, int max_learn)' in SKILL

def test_restored_combat_gain_code_is_back():
    assert 'restored_dodge_gain' in COMBAT
    assert 'restored_parry_gain' in COMBAT
    assert 'restored_hit_gain' in COMBATD

def test_interaction_buttons_use_persistent_delegation():
    assert "root.dataset.commandDelegated==='1'" in APP
    assert "e.target.closest('.context-item-action[data-command]')" in APP
    assert 'function bindContextActions(){bindCommandButtons(contextBody);}' in APP
    assert 'send(command);' in APP
