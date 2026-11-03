from pathlib import Path

MUD = Path(__file__).resolve().parents[1] / 'source' / 'upstream' / 'mudlib'


def test_devoured_player_reincarnates_in_own_context():
    s = (MUD / 'daemon/race/yaksa/devour.c').read_text(encoding='utf-8')
    body = s[s.index('if (userp(victim)) {'):]
    body = body[:body.index('return;')]
    body = '\n'.join(l.split('//')[0] for l in body.splitlines())   # 只看程式碼，不看註解
    # 選種族畫面會綁在目前下指令的人身上，不能在夜叉的動作裡直接 make_mist()。
    assert 'make_mist' not in body and 'victim->die()' not in body
    assert 'victim->consume_stat("HP", victim->query_stat("HP") + 1, me);' in body
    h = (MUD / 'std/race/humanoid.c').read_text(encoding='utf-8')
    assert 'CHAR_D->make_mist(ob);' in h


def test_devour_is_sealed():
    s = (MUD / 'daemon/race/yaksa/devour.c').read_text(encoding='utf-8')
    assert '#define DEVOUR_SEALED 1' in s
    main = s[s.index('int main(object me, string arg)'):]
    # 封印檢查要在開始吞食（設定 yaksa_devour）之前。
    assert main.index('if (DEVOUR_SEALED)') < main.index('me->set_temp("yaksa_devour"')
    seal = main[main.index('if (DEVOUR_SEALED)'):]
    seal = seal[:seal.index(';')]
    assert 'devour' not in seal.split('notify_fail', 1)[1]   # 給玩家看的話不提指令


if __name__ == '__main__':
    test_devoured_player_reincarnates_in_own_context()
    test_devour_is_sealed()
    print('ok')
