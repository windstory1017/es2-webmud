from pathlib import Path

# 玄天派武功（使用者定案，2026-11-14；docs/martial_arts/玄天教_門派設計.md）。
ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'


def read(p):
    return (MUD / p).read_text(encoding='utf-8')


def test_sky_force():
    s = read('daemon/skill/sky_force.c')
    assert 'art_id    = "sky force";' in s and 'art_usage = "force";' in s and 'art_entry = 10;' in s
    assert '#define FACTION "scholar.sky"' in s and 'me->query_level() < 5' in s
    # 每級：精 2～3、氣 2～3、神 1～2；之後 100 級起每 20 級悟性 +1、精氣神上限各 +10
    assert 'me->advance_stat("gin", 2 + random(2));' in s and 'me->advance_stat("kee", 2 + random(2));' in s
    assert 'me->advance_stat("sen", 1 + random(2));' in s
    assert '#define MILESTONE_FIRST 100' in s and '#define MILESTONE_STEP  20' in s and '#define MILESTONE_STAT  10' in s
    assert 'me->set_attr("int", me->query_attr("int", 1) + steps);' in s
    assert 'me->query("sky_force/milestones")' in s
    # 武學之道 (等級 - 50) × 10（51 級起），武術造詣 等級 × 10
    assert 'ma_coef  = 10;' in s and 'mm_start = 51;' in s and 'mm_minus = 50;' in s and 'mm_coef  = 10;' in s
    # refresh：-氣 sk/6、+精 sk；heal：-精 sk/20、+氣 5 + sk/2；都停頓 1 回合
    assert 'cost = sk / 6;' in s and 'me->supplement_stat("gin", sk);' in s
    assert 'cost = sk / 20;' in s and 'me->supplement_stat("kee", 5 + sk / 2);' in s
    assert s.count('me->start_busy(1);') == 2
    assert 'case "sky force":' in s and 'case "refresh":' in s and 'case "heal":' in s


def test_advance_sky_sword():
    s = read('daemon/skill/advance-sky_sword.c')
    assert '#define SKILL_ID  "advance-sky sword"' in s and 'art_usage = "sword";' in s
    assert 'art_entry     = 20;' in s and 'mm_start = 51;' in s and 'mm_minus = 50;' in s
    # 基本 5 招，傷害力一般
    assert s.count('"damage_type"') == 5
    for name in ('雲開見日', '玄關暗渡', '星垂平野', '天心月影', '兩儀轉乾'):
        assert '"name": "' + name + '"' in s
    # 可 enable 劍術與招架；內功是玄天心法時招架 2 倍（要戰鬥程式呼叫才生效）
    assert 'usage == "sword" || usage == "parry"' in s
    assert 'me->skill_mapped("force") == "sky force" ? 2 : 1' in s
    # perform 名稱與顏色（整句 HIW）
    assert '"困天訣", "color": HIG' in s and '"凝精訣", "color": HIY' in s
    assert '"破氣訣", "color": HIR' in s and '"隱神訣", "color": HIB' in s
    assert '"【" + p["name"] + "】"' in s and 'HIW + before' in s
    # 困天訣：成功率 sk/4 + 悟性、上限 95；消耗精最大值 1/10；對手停頓 2、自身停頓 1
    assert 'me->query_skill(SKILL_ID, 1) / 4 + me->query_attr("int")' in s and '#define HIT_CAP   95' in s
    assert 'me->query_stat_maximum("gin") / 10' in s and 'victim->start_busy(2);' in s and 'me->start_busy(1);' in s
    # break：命中率為困天 2 倍、傷害 (悟性 + sk/5) 的 2/3～全額、消耗 sen 最大值 1/8
    assert 'hold_chance(me) * 2' in s and 'top * 2 / 3' in s
    assert 'me->query_stat_maximum("sen") / 8' in s and 'me->query_stat("sen") < cost + 1' in s
    # 傷害 1/4 回自身有效上限（格子）、1/2 回現在值
    assert 'victim->consume_stat(stat, dmg, me);' in s
    assert 'me->heal_stat(stat, dmg / 4);' in s and 'me->supplement_stat(stat, dmg / 2);' in s
    # 冷卻 2 tick、各 perform 獨立
    assert '#define CD_TICKS  2' in s and 'sky_sword/cd/' in s


def test_help_files():
    assert read('docs/help/scholar.sky.txt').startswith('○ 玄天派資料')
    assert read('docs/help/advance-sky_sword.txt').startswith('◎ 玄天劍訣（advance-sky sword）')
    assert read('docs/help/sky_force.txt').startswith('◎ 玄天心法（sky force）')
    for f in ('scholar.sky.txt', 'advance-sky_sword.txt', 'sky_force.txt'):
        for line in read('docs/help/' + f).split('\n'):
            assert len(line.encode('big5', errors='replace')) <= 72, (f, line)


def test_npc_skill_faction_unchanged():
    s = read('daemon/misc/npc_skill.c')
    assert '"name":        "玄天派"' in s and '"force":       "sky force"' in s


if __name__ == '__main__':
    for name, fn in list(globals().items()):
        if name.startswith('test_') and callable(fn):
            fn()
    print('ok')
