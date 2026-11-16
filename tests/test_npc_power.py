from pathlib import Path
import re

MUD = Path(__file__).resolve().parents[1] / 'source' / 'upstream' / 'mudlib'


def read(p):
    return (MUD / p).read_text(encoding='utf-8')


def test_tier_multipliers():
    s = read('daemon/misc/npc_power.c')
    table = s[s.index('private mapping tier_pct = (['):]
    table = table[:table.index(']);')]
    assert dict(re.findall(r'"([CBAS])":\s*(\d+)', table)) == {
        'C': '70', 'B': '100', 'A': '120', 'S': '140'}
    assert '#define ATTR_CAP        45' in s


def test_class_attribute_order():
    s = read('daemon/misc/npc_power.c')
    table = s[s.index('private mapping class_attr = (['):]
    table = table[:table.index(']);')]
    got = {c: re.findall(r'"([a-z]+)"', a)
           for c, a in re.findall(r'"([a-z]+)":\s*\(\{([^}]*)\}\)', table)}
    assert got == {
        'fighter': ['con', 'str', 'cor'],
        'scholar': ['int', 'dex', 'con'],
        'soldier': ['str', 'cor', 'cps'],
        'taoist': ['wis', 'spi', 'int'],
        'monk': ['spi', 'wis', 'int'],
        'alchemist': ['int', 'spi', 'str'],
        'thief': ['cor', 'dex', 'cps'],
        'commoner': [],
    }


def test_npc_hooks():
    s = read('std/char/npc.c')
    assert 'void set_power(string tier)' in s
    assert 'if( !applying_power ) manual_attr[what] = 1;' in s
    assert 'if( !applying_power ) manual_stat[what] = 1;' in s
    assert 'NPC_POWER_D->apply_power(this_object(), power_tier, manual_attr, manual_stat);' in s
    assert '#define NPC_POWER_D\t\t"/daemon/misc/npc_power.c"' in read('include/daemon.h')
    assert '/daemon/misc/npc_power' in read('adm/etc/preload').splitlines()


def calc_attr(base, rate, lv, pct):
    v = base + ((lv - 1) * rate * pct + 5000) // 10000
    return min(v, max(base, 45))


def test_formula_examples():
    # 黑齒族武者 LV10 C 級（種族平均 con 15.5 / str 16.5 / cor 17.5）
    assert calc_attr(15, 43, 10, 70) == 18
    assert calc_attr(16, 36, 10, 70) == 18
    assert calc_attr(17, 29, 10, 70) == 19
    # B 級第 1 重要屬性約在 LV70 到頂
    assert calc_attr(15, 43, 70, 100) == 45
    assert calc_attr(15, 10, 70, 100) == 22


def test_existing_npcs_converted():
    # 已套用的 NPC 不再手動寫死屬性或精氣神上限。
    import re as _re
    listed = _re.findall(r'\| `([^`]+)` \| [^|]+ \| (\d+) \| ([CBAS]) \|',
                         (MUD.parents[2] / 'docs' / 'NPC_POWER_LIST.md').read_text(encoding='utf-8'))
    assert len(listed) == 87
    for f, lv, tier in listed:
        s = read(f)
        assert f'set_power("{tier}");' in s, f
        assert _re.search(r'(?<![>\w])set_level\s*\(%s\)' % lv, s), f
        assert not _re.search(r'^\s*set_attr\(', s, _re.M), f
        if f == 'd/wutang/npc/may_yin_fong.c':   # 梅影風：精氣神固定 2200／2500／800（使用者指示，2026-11-12）
            continue
        assert not _re.search(r'^\s*set_stat_maximum\("(gin|kee|sen)"', s, _re.M), f
        assert not _re.search(r'^\s*advance_stat\("(gin|kee|sen)"', s, _re.M), f


def test_elite_bonus():
    s = read('daemon/misc/npc_power.c')
    assert 'private string *elite_tier = ({ "A", "S" });' in s
    assert '#define ELITE_BONUS_FROM    20' in s
    assert '#define ELITE_BONUS_PER     10' in s
    assert 'return (lv - ELITE_BONUS_FROM) * ELITE_BONUS_PER;' in s
    assert '(gain[i] * pct + 50) / 100 + bonus);' in s
