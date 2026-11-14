# 黃白之術 2026-11-14 修改：比例、80 級卡住升 100、銀咒總額 2000、一次轉多個。
# 設計文件：docs/martial_arts/龍圖丹派_黃白之術.md
from pathlib import Path

MUD = Path(__file__).resolve().parents[1] / 'source' / 'upstream' / 'mudlib'


def read(p):
    return (MUD / p).read_text(encoding='utf-8')


def wealth_rate(level):
    # 與 daemon/skill/alchemy-wealth.c 的 wealth_rate() 相同（千分之一）
    if level < 20:
        return 600
    rate = 700 + (min(level, 140) - 20) // 5 * 10
    if level > 140:
        rate += (level - 140) * 35
    return rate


def test_rate_table():
    assert [wealth_rate(x) for x in (0, 20, 25, 80, 100, 140, 141, 200)] == \
        [600, 700, 710, 820, 860, 940, 975, 3040]
    s = read('daemon/skill/alchemy-wealth.c')
    assert 'if( level < 20 ) return 600;' in s
    assert 'rate = 700 + ((level > 140 ? 140 : level) - 20) / 5 * 10;' in s
    assert 'if( level > 140 ) rate += (level - 140) * 35;' in s
    assert 'money = total * wealth_rate(level) / 1000;' in s


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
