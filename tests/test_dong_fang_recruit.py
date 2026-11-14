from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'


def read(p):
    return p.read_text(encoding='utf-8')


def test_dong_fang_recruits_commoner_or_scholar():
    s = read(MUD / 'custom/legacy_npc/dong_fang_gu_ming.c')
    assert '#define FACTION "scholar.sky"' in s
    assert 'int accept_apprentice(object me)' in s and 'literate' not in s   # 不考慮讀書識字
    assert 'cls != "commoner"' in s and 'cls == "scholar"' in s
    assert 'strsrch(title, "步玄")' in s                                      # 步玄派的不收
    assert 'me->set("title", "玄天派弟子");' in s and 'me->set("custom_faction", FACTION);' in s
    assert 'me->set_class("scholar");' in s


def test_dong_fang_teaches_only_disciples():
    s = read(MUD / 'custom/legacy_npc/dong_fang_gu_ming.c')
    assert 'if( me->query("custom_faction") != FACTION ) return 0;' in s
    for sk in ('advance-sky sword', 'sky force'):
        assert 'case "%s":' % sk in s
        assert (MUD / 'daemon/skill' / (sk.replace(' ', '_') + '.c')).exists()
    assert 'me->query_level() < 5' in s       # 玄天心法要等級 5
    assert '/custom/legacy_npc/dong_fang_gu_ming' in read(MUD / 'd/fengcheng/inn2.c')


def test_help_no_literate_requirement():
    assert '讀書識字' not in read(MUD / 'docs/help/scholar.sky.txt')
