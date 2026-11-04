from pathlib import Path

# 風嘯拳法任務：玩家進醇雨樓客棧時雍泰打聽四弟，每句間隔 1 tick（使用者指示，2026-11-04）。
MUD = Path(__file__).resolve().parents[1] / 'source' / 'upstream' / 'mudlib'


def read(p):
    return (MUD / p).read_text(encoding='utf-8')


def test_greeting_on_enter():
    s = read('d/wutang/npc/yung_tai.c')
    assert 'void init()' in s and '#define TICK 2' in s
    assert 'call_out("greet", TICK, who, 0);' in s and 'call_out("greet", TICK, who, step);' in s
    assert 'who->query_skill("fonxiaoken", 1)' in s
    for line in ['這位朋友可是外地來的﹖', '是我那失散的四弟 ...', '前年在斐縣境中失散 ...',
                 '也不知道是生是死 ...', '請千萬讓小弟知道 ...', '又繼續喝酒。']:
        assert line in s, line
    assert '"/d/wutang/npc/yung_tai" : 1,' in read('d/wutang/inn.c').replace('__DIR__"', '"/d/wutang/')


if __name__ == '__main__':
    test_greeting_on_enter()
    print('ok')
