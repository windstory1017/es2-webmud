from pathlib import Path
import re

MUD = Path(__file__).resolve().parents[1] / 'source' / 'upstream' / 'mudlib'


def read(p):
    return (MUD / p).read_text(encoding='utf-8')


def test_ashura_fighter_cap_70():
    s = read('daemon/race/ashura.c')
    caps = dict(re.findall(r'"([a-z]+)":\s*(-?\d+)', s[s.index('class_level_cap'):]))
    assert caps['fighter'] == '70'
    assert caps['soldier'] == '70' and caps['taoist'] == '60'


def test_level_cap_helper_used_by_gain_and_score():
    s = read('feature/char/score.c')
    assert 'int at_level_cap()' in s
    assert 'return cap != 0 && query_level() >= cap;' in s
    assert 'if( at_level_cap() ) return 0;' in s
    sc = read('cmds/usr/score.c')
    assert '!ob->at_level_cap() && (xp_t = ob->query_target_score(c) - xp) > 0' in sc


def test_commoner_levels_on_survive_only():
    s = read('daemon/class/commoner.c')
    assert '#include <class_level.h>' in s
    assert 'ob->set_target_score("survive", sq_from(cur_lv + 1, 1) * 100 * race_coef(ob) / 100);' in s
    assert 'grow_stats(' not in s.split('void advance_level')[1]
    assert s.count('set_target_score(') == 1


def test_commoner_cap_50_for_all_races():
    s = read('feature/char/score.c')
    assert '#define COMMONER_LEVEL_CAP  50' in s
    assert 'if( query_class() == "commoner" )\n        cap = COMMONER_LEVEL_CAP;' in s
