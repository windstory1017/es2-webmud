// 善想禪要（absorption）。依使用者設計表（2026-10-16）。
//
// 1. 每升到 5 的倍數級，有機會增加慧根：沿用技能成長屬性的規則
//    （feature/char/skill.c 的 ATTR_GROWTH 表，absorption → wis）。
// 3. 打坐（zazen，使用者定案，2026-10-19）：和尚才能打坐。每 tick 消耗神
//    （10 − (random(慧根)+1)/8，最少 0），
//    每 3 tick 神上限 −1；神低於 10 時出定。每 tick 得 慧根版智力公式 的點數，
//    每 2 tick 得 (random(慧根)+1)/6 禪定修養，並在自己畫面顯示一句心經。
//    100 級時慧根固定 +3（這一級不再擲每 5 級的慧根機率）。
// 2. 每升到第 sk 級，獲得 sk × 30 點禪定修養（cultivation）與 sk × 30 點佛學修為（buddhology）。
//    一次升多級時每一級都給。只對玩家生效。
//    已經給到哪一級記在 skill_bonus/absorption，同一級一生只給一次。

#include <ansi.h>

inherit SKILL;

#define CULTIVATION_PER_LEVEL   30

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("absorption");
    setup();
}

void skill_improved(object me, string skill)
{
    me->apply_restored_skill_progression(skill);
}

void skill_advanced(object me, string skill)
{
    int done, now, lv, gain;

    if( !userp(me) ) return;

    now = me->query_skill(skill, 1);
    if( now >= 100 && !me->query("skill_bonus/absorption_100") ) {
        me->set("skill_bonus/absorption_100", 1);
        me->set_attr("wis", me->query_attr("wis", 1) + 3);
        tell_object(me, HIY "靜坐之中﹐你忽覺萬念俱寂﹐心如明鏡不染纖塵﹐"
            "一念不生而萬法自現──善想禪要已臻圓融之境。\n" NOR);
    }
    done = me->query("skill_bonus/absorption");
    if( now <= done ) return;

    me->set("skill_bonus/absorption", now);
    for(lv = done + 1; lv <= now; lv++) gain += lv * CULTIVATION_PER_LEVEL;
    if( gain > 0 ) {
        me->gain_score("cultivation", gain);
        // 同時獲得同量的佛學修為（每升到第 sk 級 sk × 30，使用者定案，2026-10-16）。
        me->gain_score("buddhology", gain);
    }
}

// ------------------------------------------------------------------
// 打坐
// ------------------------------------------------------------------

string *SUTRA = ({
    "觀自在菩薩。行深般若波羅蜜多時。",
    "照見五蘊皆空。度一切苦厄。",
    "舍利子。色不異空。空不異色。",
    "色即是空。空即是色。",
    "受想行識。亦復如是。",
    "舍利子。是諸法空相。",
    "不生不滅。不垢不淨。不增不減。",
    "是故空中無色。無受想行識。",
    "無眼耳鼻舌身意。無色聲香味觸法。",
    "無眼界。乃至無意識界。",
    "無無明。亦無無明盡。",
    "乃至無老死。亦無老死盡。",
    "無苦集滅道。無智亦無得。以無所得故。",
    "菩提薩埵。依般若波羅蜜多故。",
    "心無罣礙。無罣礙故。無有恐怖。",
    "遠離顛倒夢想。究竟涅槃。",
    "三世諸佛。依般若波羅蜜多故。",
    "得阿耨多羅三藐三菩提。",
    "故知般若波羅蜜多。",
    "是大神咒。是大明咒。",
    "是無上咒。是無等等咒。",
    "能除一切苦。真實不虛。",
    "故說般若波羅蜜多咒。即說咒曰。",
    "揭諦揭諦　波羅揭諦　波羅僧揭諦　菩提薩婆訶。",
});

private void end_zazen(object me, int tired)
{
    me->delete_temp("zazen");
    if( tired )
        message_vision("$N只覺心神漸感疲憊﹐再也守不住定境﹐於是緩緩收攝心念﹐慢慢張開雙眼。\n", me);
    else
        message_vision("$N緩緩吐出一口長氣﹐慢慢張開雙眼﹐眉宇之間一片澄明。\n", me);
}

int do_zazen(object me)
{
    int tick, wis, w, cost, n;

    if( me->query_stat("sen") < 10 ) {
        end_zazen(me, 1);
        return 0;
    }

    tick = me->add_temp("zazen/tick", 1);
    wis = me->query_attr("wis");
    w = wis > 0 ? wis : 1;

    cost = 10 - (random(w) + 1) / 8;
    if( cost > 0 ) me->consume_stat("sen", cost);
    if( tick % 3 == 0 ) me->damage_stat("sen", 1);

    me->improve_skill_exact("absorption", (random(w) + 1) * (random(w) + 1) + random(w));

    if( tick % 2 == 0 ) {
        if( (n = (random(w) + 1) / 6) > 0 ) me->gain_score("cultivation", n);
        // 心經只有打坐的人自己看到（使用者定案，2026-10-19）。
        tell_object(me, CYN + SUTRA[(tick / 2 - 1) % sizeof(SUTRA)] + "\n" NOR);
    }

    if( me->query_stat("sen") < 10 ) {
        end_zazen(me, 1);
        return 0;
    }
    return 1;
}

int halt_zazen(object me, object who, string how)
{
    end_zazen(me, 0);
    return 1;
}

int start_zazen(object me)
{
    if( me->query_class() != "monk" )
        return notify_fail("你並非佛門弟子﹐無從參習善想禪要。\n");
    if( !me->query_skill("absorption", 1) && !me->query_learn("absorption") )
        return notify_fail("你尚未得授善想禪要的法門。\n");
    if( me->is_fighting() )
        return notify_fail("刀光劍影之中﹐如何靜得下心來打坐？\n");
    if( me->is_busy() )
        return notify_fail("你正忙著別的事﹐靜不下心來。\n");
    if( me->query_stat("sen") < 10 )
        return notify_fail("你心神太過疲憊﹐一時無法入定。\n");

    me->set_temp("zazen/tick", 0);
    message_vision("$N盤膝而坐﹐雙手結印輕置膝上﹐垂目斂息﹐漸漸入定。\n", me);
    me->start_busy((: do_zazen, me :), (: halt_zazen, me :));
    return 1;
}
