from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'


def read(p):
    return p.read_text(encoding='utf-8')


S = read(MUD / 'daemon/skill/nine-dragon_force.c')


def test_basic_data():
    assert 'art_id    = SKILL_ID;' in S and '#define SKILL_ID  "nine-dragon force"' in S
    assert 'art_usage = "force";' in S and 'art_entry = 5;' in S
    assert '"nine-dragon force":"九龍神功"' in read(MUD / 'data/chinese.o')


def test_learning_conditions():
    assert 'is_faction(me, FACTION)' in S and '#define FACTION   "taoist.thunder"' in S
    assert 'me->query_level() < 10' in S


def test_level_growth_and_milestone():
    assert 'me->advance_stat("gin", 1 + random(2));' in S
    assert 'me->advance_stat("kee", 2);' in S and 'me->advance_stat("sen", 1 + random(3));' in S
    assert 'lv >= 100 && !me->query("nine_dragon/milestone")' in S
    assert 'me->set_attr("con", me->query_attr("con", 1) + 2);' in S
    assert 'me->set_attr("wis", me->query_attr("wis", 1) + 1);' in S


def test_exercise_cost():
    assert 'me->consume_stat("gin", 2);' in S and 'me->damage_stat("gin", 1);' in S
    assert 'me->consume_stat("kee", 2);' in S and 'me->damage_stat("kee", 1);' in S
    assert 'me->consume_stat("sen", 3);' in S and 'me->damage_stat("sen", 2);' in S


def test_heal_regenerate_overload():
    assert 'cost = 5 + sk / 20;' in S and 'me->supplement_stat("kee", 10 + sk / 2);' in S
    assert 'cost = 5 + sk / 10;' in S and 'me->supplement_stat("sen", 10 + sk / 2);' in S
    assert 'me->consume_stat("sen", 75);' in S and 'me->set_temp("taoism/overload", 1);' in S
    assert S.count('me->start_busy(1);') == 3          # 三個功能都停頓 1 tick
    spells = read(MUD / 'daemon/skill/taoism-spells.c')
    assert 'me->query_temp("taoism/overload")' in spells and 'D = D * 3 / 2;' in spells   # 天師道法已讀取增幅


def test_hua_zi_yi_teaches_it():
    h = read(MUD / 'd/fengcheng/npc/hua_zi_yi.c')
    assert 'file_size("/daemon/skill/nine-dragon_force.c") < 0' in h and 'me->query_level() < 10' in h
    assert (MUD / 'daemon/skill/nine-dragon_force.c').exists()
    assert (MUD / 'docs/help/nine-dragon_force.txt').exists()
    assert '（nine-dragon force）' in read(MUD / 'docs/help/taoist.thunder.txt')
