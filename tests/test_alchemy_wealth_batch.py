# 黃白之術 2026-11-14 修改：比例、丹藥加成、80 級卡住升 100、銀咒總額 2000、一次轉多個；櫃台停賣大還丹。
# 設計文件：docs/martial_arts/龍圖丹派_黃白之術.md
from pathlib import Path

MUD = Path(__file__).resolve().parents[1] / 'source' / 'upstream' / 'mudlib'


def read(p):
    return (MUD / p).read_text(encoding='utf-8')


def wealth_rate(level):
    # 與 daemon/skill/alchemy-wealth.c 的 wealth_rate() 相同（百分比，整數）
    if level < 20:
        return 60
    if level < 100:
        return 70 + (min(level, 80) - 20) // 5
    return level


def test_rate_table():
    assert [wealth_rate(x) for x in (0, 20, 25, 50, 80, 95, 100, 150, 200)] == \
        [60, 70, 71, 76, 82, 82, 100, 150, 200]
    s = read('daemon/skill/alchemy-wealth.c')
    assert 'if( level < 20 ) return 60;' in s
    assert 'if( level < 100 ) return 70 + ((level > 80 ? 80 : level) - 20) / 5;' in s
    assert 'money = total * wealth_rate(level) * pill_bonus(target) / 100;' in s


def test_pill_bonus_and_grades():
    s = read('daemon/skill/alchemy-wealth.c')
    assert 'case 1: return 10;' in s and 'case 2: return 2;' in s
    assert 'set("pill_grade", 2)' in read('obj/alchemist/green_pill.c')
    assert 'set("pill_grade", 1)' in read('obj/alchemist/major_heal_pill.c')


def test_herb_counter_stops_selling_major():
    c = read('daemon/misc/herb_catalog.c')
    assert 'int sells_product(string key) { return key != "major"; }' in c
    assert 'if (mode == "buy") return sells_product(key) ? base * 10 : -1;' in c
    d = read('daemon/misc/herb_d.c')
    assert 'if (!CATALOG->sells_product(key)) return say_to(me, "藥鋪不販售" + name + "。");' in d
    assert '"不販售"' in d


def test_levels_and_limits():
    s = read('daemon/skill/alchemy-wealth.c')
    for line in ['#define SILVER_LIMIT 2000', '#define GOLD_LEVEL   80',
                 '#define HOLD_LEVEL   80', '#define JUMP_LEVEL   100']:
        assert line in s
    assert 'me->skill_threshold(SKILL_ID, JUMP_LEVEL)' in s
    assert 'me->set_skill(sk, JUMP_LEVEL);' in s


def test_batch_and_message():
    s = read('daemon/skill/alchemy-wealth.c')
    assert 'me->query_temp("conjure/amount")' in s
    assert '錢莊存款增加' not in s
    assert 'reward_success(me, total);' in s
    c = read('cmds/std/conjure.c')
    assert 'sscanf(trg, "all %s", rest)==1' in c
    assert 'me->set_temp("conjure/amount", amount);' in c
    assert 'me->delete_temp("conjure/amount");' in c


if __name__ == '__main__':
    for name, fn in list(globals().items()):
        if name.startswith('test_'):
            fn()
    print('ok')
