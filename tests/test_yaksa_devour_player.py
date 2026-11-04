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
    # 封印開關：2026-11-04 重新開放（0）；封印檢查本身保留，要再封印改成 1。
    s = (MUD / 'daemon/race/yaksa/devour.c').read_text(encoding='utf-8')
    assert '#define DEVOUR_SEALED 0' in s
    main = s[s.index('int main(object me, string arg)'):]
    # 封印檢查要在開始吞食（設定 yaksa_devour）之前。
    assert main.index('if (DEVOUR_SEALED)') < main.index('me->set_temp("yaksa_devour"')
    seal = main[main.index('if (DEVOUR_SEALED)'):]
    seal = seal[:seal.index(';')]
    assert 'devour' not in seal.split('notify_fail', 1)[1]   # 給玩家看的話不提指令


def test_help_block_fits_driver_limit():
    # 驅動程式的 @ 文字區塊上限是 1024 位元組（neolith lib/lpc/lex.h MAXLINE），超過會編譯失敗。
    s = (MUD / 'daemon/race/yaksa/devour.c').read_text(encoding='utf-8')
    block = s[s.index('@HELP\n') + len('@HELP\n'):s.index('\nHELP);')]
    assert len(block.encode('utf-8')) < 1000


def test_threshold_rules():
    s = (MUD / 'daemon/race/yaksa/devour.c').read_text(encoding='utf-8')
    assert 'int extra = count >= 10 ? count - 9 : 0;' in s
    assert 'return yaksa_ghost ? 250 + 25 * extra : 500 + 50 * extra;' in s
    assert '#define COUNT_KEY "yaksa/devour_count"' in s
    body = s[s.index('private void reward_player'):s.index('private void finish')]
    # 未達門檻：不計數、不加成；達到門檻才 add 次數，且比完門檻才加倍。
    # 夜叉自己的精（最大值）也要達到同一個門檻。
    assert 'own_gin = me->query_stat_maximum("gin");' in body
    assert 'if (ghost_gin < need || own_gin < need) {' in body
    miss = body[body.index('if (ghost_gin < need || own_gin < need)'):body.index('} else {')]
    assert 'COUNT_KEY' not in miss and 'grow(' not in miss
    assert body.index('if (ghost_gin < need || own_gin < need)') < body.index('ghost_gin *= 2')
    assert 'devour_log(' in body
    # 未達門檻一樣魂飛魄散。
    fin = s[s.index('private void finish'):]
    fin = fin[fin.index('if (userp(victim)) {'):]
    fin = fin[:fin.index('return;')]
    assert 'reward_player(' in fin and 'consume_stat("HP"' in fin


def test_devour_log_path_is_writable_by_mudlib():
    s = (MUD / 'daemon/race/yaksa/devour.c').read_text(encoding='utf-8')
    assert '#define DEVOUR_LOG "/data/daemon/yaksa_devour.log"' in s
    sec = (MUD / 'adm/daemons/securityd.c').read_text(encoding='utf-8')
    assert '"/data/daemon":\t({ MUDLIB_UID }),' in sec


def test_first_bite_delayed_two_ticks():
    s = (MUD / 'daemon/race/yaksa/devour.c').read_text(encoding='utf-8')
    main = s[s.index('int main(object me, string arg)'):]
    assert 'call_out("devour_tick", TICK_SECONDS * 2, me, victim, serial);' in main
    assert 'devour_tick(me, victim, serial);' not in main


def test_small_temple_is_no_fight():
    s = (MUD / 'd/snow/small_temple.c').read_text(encoding='utf-8')
    assert 'set ("no_fight", 1);' in s


if __name__ == '__main__':
    test_devoured_player_reincarnates_in_own_context()
    test_devour_is_sealed()
    test_help_block_fits_driver_limit()
    test_threshold_rules()
    test_devour_log_path_is_writable_by_mudlib()
    test_first_bite_delayed_two_ticks()
    test_small_temple_is_no_fight()
    print('ok')
