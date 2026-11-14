from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'


def read(p):
    return p.read_text(encoding='utf-8')


def test_hua_zi_yi_recruits_commoner_with_wisdom():
    s = read(MUD / 'd/fengcheng/npc/hua_zi_yi.c')
    assert '#define FACTION "taoist.thunder"' in s and 'set("custom_faction", FACTION);' in s
    assert 'me->query_class() != "commoner"' in s                      # 只收平民，已有師承的不收
    assert 'me->query_attr("wis", 1) < 16' in s                         # 慧根至少 16
    assert 'me->set("title", "紫衣派弟子");' in s and 'me->set_class("taoist");' in s


def test_hua_zi_yi_teaches_only_disciples():
    s = read(MUD / 'd/fengcheng/npc/hua_zi_yi.c')
    assert 'if( me->query("custom_faction") != FACTION ) return 0;' in s
    for sk in ('taoism-thunder', 'reflecting star sword', 'magic steps', 'nine-dragon force'):
        assert 'case "%s":' % sk in s
    assert 'me->query_level() < 10' in s                                # 九龍神功與內功要等級 10
    npc_skill = read(MUD / 'daemon/misc/npc_skill.c')
    assert '"taoist.thunder"' in npc_skill and '"force":       "nine-dragon force"' in npc_skill
