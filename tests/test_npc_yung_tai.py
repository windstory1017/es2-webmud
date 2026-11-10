from pathlib import Path

# 風嘯拳法任務（使用者指示，2026-11-04～05；docs/martial_arts/共通_風嘯拳法.md「任務」）。
MUD = Path(__file__).resolve().parents[1] / 'source' / 'upstream' / 'mudlib'


def read(p):
    return (MUD / p).read_text(encoding='utf-8')


def test_greeting_on_enter():
    s = read('d/wutang/npc/yung_tai.c')
    assert 'void init()' in s and '::init();' in s and '#define TICK        2' in s
    assert 'call_out("greet", TICK, who, 0);' in s and 'call_out("greet", TICK, who, step);' in s
    assert 'query_class' not in s and 'undefinedp(who->query_skills()[QUEST_SKILL])' in s
    for line in ['這位朋友可是外地來的﹖', '是我那失散的四弟 ...', '前年在斐縣境中失散 ...',
                 '也不知道是生是死 ...', '請千萬讓小弟知道 ...', '又繼續喝酒。']:
        assert line in s, line


def test_ring_urn_and_teaching():
    s = read('d/wutang/npc/yung_tai.c')
    assert '"/d/graveyard/obj/chrysoberyl_ring"' in s and '"/d/graveyard/obj/urn"' in s
    assert 'strsrch(msg, "雍順")' in s
    assert '#define FAINT_TICKS 5' in s and 'call_out("revive", FAINT_TICKS * TICK);' in s
    assert 'who->query_temp("yung_tai/ring")' in s
    assert 'who->improve_skill_exact(QUEST_SKILL, 1);' in s
    for line in ['怔怔地流下淚來。', '只剩下我一個人了', '家傳的拳法傳給恩公作為答謝之禮。',
                 '(雍泰將風嘯拳法的秘要盡數傳了給你)']:
        assert line in s, line
    for f in ('d/graveyard/obj/chrysoberyl_ring.c', 'd/graveyard/obj/urn.c'):
        assert (MUD / f).exists(), f


def test_dirty_taoist_gets_drunk():
    s = read('d/graveyard/npc/dirty_taoist.c')
    assert '#define DRUNK_SIPS  3' in s and '#define DRUNK_TICKS 30' in s
    assert '(: drink :)' in s and 'unconcious();' in s and 'call_out("revive", DRUNK_TICKS * TICK);' in s
    assert 'chrysoberyl_ring")->wear();' in s and 'obj/urn");' in s


def test_fonxiaoken_skill():
    s = read('daemon/skill/fonxiaoken.c')
    assert 'art_usage = "unarmed";' in s and s.count('"action":') == 6 and s.count('"name":') == 4
    assert 'art_entry        = 5;' in s and 'mm_start = 21;' in s and 'mm_minus = 20;' in s and 'mm_coef  = 15;' in s
    assert '#define SPECIAL_LEVEL 90' in s and 'return 100 + 50 * (lv - SPECIAL_LEVEL) / (200 - SPECIAL_LEVEL);' in s
    assert 'me->supplement_stat("gin", SPECIAL_GIN);' in s and 'opponent->start_busy(1);' in s
    assert 'HIW "$N吐氣開聲' in s and 'me->start_busy' not in s
    h = read('docs/help/fonxiaoken.txt')
    assert h.startswith('◎ 風嘯拳法（fonxiaoken）') and '任務起始：五堂鎮醇雨樓客棧 雍泰' in h
    assert (MUD.parents[2] / 'docs/martial_arts/共通_風嘯拳法.md').exists()


if __name__ == '__main__':
    for name, fn in list(globals().items()):
        if name.startswith('test_') and callable(fn):
            fn()
    print('ok')
