from pathlib import Path
import re

MUD = Path(__file__).resolve().parents[1] / 'source' / 'upstream' / 'mudlib'


def read(p):
    return (MUD / p).read_text(encoding='utf-8')


def table(s, name):
    t = s[s.index(name):]
    return t[:t.index(']);')]


def test_coefficients_and_cap():
    s = read('daemon/misc/npc_skill.c')
    assert '#define SKILL_CAP       200' in s
    assert '#define MAIN_PCT        300' in s
    assert '#define COMMON_PCT      200' in s
    assert '#define AUX_PCT         150' in s


def test_class_skill_table():
    t = table(read('daemon/misc/npc_skill.c'), 'private mapping class_skill = ([')
    got = {}
    for cls, main, aux in re.findall(
            r'"([a-z]+)":\s*\(\[\s*"main":\s*\(\{([^}]*)\}\),\s*"aux":\s*\(\{([^}]*)\}\)', t):
        got[cls] = (re.findall(r'"([^"]+)"', main), re.findall(r'"([^"]+)"', aux))
    assert got == {
        'fighter': ([], []),
        'soldier': (['berserk'], []),
        'thief': (['backstab', 'killerhood'], ['sneak', 'dagger']),
        'taoist': (['spells'], []),
        'scholar': (['dodge', 'literate', 'archaic attainment'], []),
        'monk': (['magic'], []),
        'alchemist': (['magic'], []),
    }
    assert 'commoner' not in got


def test_sect_forces():
    t = table(read('daemon/misc/npc_skill.c'), 'private mapping sect = ([')
    assert re.search(r'"fighter\.tiger":.*?"force":\s*"tiger force",\s*"force_level": 15', t, re.S)
    assert re.search(r'"fighter\.lunmay":.*?"force":\s*"hainmay force",\s*"force_level": 15', t, re.S)


def test_npc_hooks():
    s = read('std/char/npc.c')
    assert 'if( !applying_power ) manual_skill[skill] = 1;' in s
    assert 'if( !applying_power ) manual_map[skill] = 1;' in s
    assert 'NPC_SKILL_D->apply_skills(this_object(), manual_skill, manual_map);' in s
    assert 'call_out("apply_npc_weapon_skills", 0);' in s
    assert '(: command, "berserk" :)' in s
    assert '#define NPC_SKILL_D\t\t"/daemon/misc/npc_skill.c"' in read('include/daemon.h')
    assert '/daemon/misc/npc_skill' in read('adm/etc/preload').splitlines()


def calc(lv, pct):
    return min(200, max(1, lv * pct // 100))


def test_formula_examples():
    assert [calc(lv, 300) for lv in (10, 30, 50, 70)] == [30, 90, 150, 200]
    assert [calc(lv, 200) for lv in (10, 30, 50, 70)] == [20, 60, 100, 140]
    assert [calc(lv, 150) for lv in (10, 30, 50, 70)] == [15, 45, 75, 105]


def test_sect_npcs_marked():
    for f, fac in (('d/oldpine/npc/kao_shen.c', '"fighter.tiger"'),
                   ('d/wutang/npc/yang_zlin.c', '"fighter.lunmay"'),
                   ('d/wutang/npc/apprentice.c', '"fighter.lunmay"'),
                   ('d/wutang/npc/may_yin_fong.c', 'FACTION')):
        s = read(f)
        assert 'set("custom_faction", %s);' % fac in s, f
        assert s.index('set("custom_faction"') < s.index('set_power('), f


def test_force_map_only_skips_manual_map():
    s = read('daemon/misc/npc_skill.c')
    assert 'if( !manual_map[sk] ) ob->map_skill(sk, r["map"][sk]);' in s


def test_sect_table_keys_and_arts():
    t = table(read('daemon/misc/npc_skill.c'), 'private mapping sect = ([')
    keys = re.findall(r'^    "([a-z]+\.[a-z]+)": \(\[', t, re.M)
    assert keys == ['fighter.lunmay', 'fighter.tiger', 'fighter.fonxan', 'fighter.chin', 'fighter.snow',
                    'fighter.ancient', 'scholar.sky', 'thief.longye', 'thief.hidden', 'taoist.fire',
                    'taoist.freeze', 'taoist.storm', 'taoist.thunder', 'taoist.fog', 'taoist.wind',
                    'taoist.lingyun']
    assert '"skill": "advance_lunmay", "map": "secondhand sword", "level": 30, "req": ([ "hainmay force": 100 ])' in t
    assert '"skill": "ksanmando", "map": "blade", "level": 1' in t


def test_unbuilt_arts_not_mapped():
    s = read('daemon/misc/npc_skill.c')
    assert 'if( !maps[art["map"]] && has_art(art["skill"]) ) maps[art["map"]] = art["skill"];' in s
    assert 'if( has_art(s["force"]) ) maps["force"] = s["force"];' in s


def test_new_chinese_names():
    s = read('data/chinese.o')
    for k, v in (('sky force', '玄天心法'), ('devil force', '大邪心經'), ('tidy force', '凌潮心法'),
                 ('nine-dragon force', '九龍神功'), ('reflecting star sword', '清河映星劍'), ('enchance', '強化之術')):
        assert '"%s":"%s"' % (k, v) in s


def test_user_decisions_2026_10_14():
    t = table(read('daemon/misc/npc_skill.c'), 'private mapping sect = ([')
    assert '"skill": "lunmay", "map": "parry", "level": 1' in t
    assert '"skill": "tiger-blade", "map": "twohanded blade", "level": 1, "req": ([ "tiger force": 30 ])' in t
    assert '"skill": "tiger-steps", "map": "dodge", "level": 1' in t
    s = read('daemon/misc/npc_skill.c')
    assert 'if( skills[rq] < 1 ) ok = 0;' in s                       # 前置武功學得到即視為練滿
    assert 'if( !maps[art["map"]] && has_art(art["skill"]) )' in s     # 以先列的為主
    assert '"devil force": 10,' in read('custom/weapon/blade/green_blade.c')
    for f, fac in (('custom/taoism/npc/fire_taoist.c', 'taoist.fire'), ('custom/taoism/npc/freeze_taoist.c', 'taoist.freeze'),
                   ('custom/taoism/npc/storm_taoist.c', 'taoist.storm'), ('custom/taoism/npc/thunder_taoist.c', 'taoist.thunder'),
                   ('d/wutang/npc/guo_boo.c', 'taoist.fire'), ('d/wutang/npc/red_taoist.c', 'taoist.fire'),
                   ('d/wutang/npc/white_taoist.c', 'taoist.freeze')):
        assert 'set("custom_faction", "%s");' % fac in read(f), f


def test_renamed_skill_ids():
    # 三門刀法、瘋虎功改用對照表的代碼；舊存檔讀進來時轉換（使用者決定，2026-10-14）。
    assert (MUD / 'daemon/skill/ksanmando.c').exists() and not (MUD / 'daemon/skill/sanmeendo.c').exists()
    assert (MUD / 'daemon/skill/tiger_force.c').exists() and not (MUD / 'daemon/skill/tiger-force.c').exists()
    assert 'register_skill_daemon("ksanmando")' in read('daemon/skill/ksanmando.c')
    assert 'register_skill_daemon("tiger force")' in read('daemon/skill/tiger_force.c')
    s = read('feature/char/skill.c')
    assert '"sanmeendo":   "ksanmando",' in s and '"tiger-force": "tiger force",' in s
    assert 'void migrate_skill_ids()' in s
    assert 'this_object()->migrate_skill_ids();' in read('feature/save.c')
