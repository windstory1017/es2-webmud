/*---
description: 凌潮心法（NEW 新版武功寫法）。雪吟莊基本心法，enable 在 force 上使用。
             exert tidy force／refresh。設計見 docs/martial_arts/雪吟莊_武功.md。
---*/
#include <ansi.h>

inherit "/std/martial_art";

#define FACTION "fighter.snow"

private void create()
{
    seteuid(getuid());

    art_id    = "tidy force";
    art_name  = "凌潮心法";
    art_usage = "force";
    art_desc  = "雪吟莊的基本心法，真氣一起一落有如潮汐，可運氣回精；"
                + "練到極處，是修習天邪神功的根基。";

    art_entry = 5;

    ma_coef  = 10;
    mm_start = 30;
    mm_minus = 29;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("tidy force");
    setup();
}

/* 學習條件：雪吟莊弟子、人物等級 5。 */
int valid_learn(object me)
{
    if( !is_faction(me, FACTION) )
        return notify_fail("你不是雪吟莊弟子﹐無法修練凌潮心法。\n");
    if( me->query_level() < 5 )
        return notify_fail("你的根基尚淺﹐還領會不了凌潮心法的運氣法門。\n");
    return 1;
}

/* 內功不會拿來出招。 */
void attack_using(object me, object opponent, object weapon) { }

/* 學成：累積到 5 級門檻，gain 時直接練成 5 級。 */
void skill_completed(object me, string skill)
{
    tell_object(me, HIC "你練成了凌潮心法﹐丹田真氣一起一落﹐如潮汐般綿綿不絕。\n" NOR);
}

/* 每級獎勵（引擎）；練成之後每升一級：精上限 +2、氣上限 +0～1。 */
void skill_advanced(object me, string skill)
{
    ::skill_advanced(me, skill);

    if( userp(me) && me->query_skill("tidy force", 1) > 5 ) {
        me->advance_stat("gin", 2);
        me->advance_stat("kee", random(2));
    }
}

/* 修習：每次運功消耗目前的精 2、氣 2，精、氣上限各受傷 1；
 * 得到 基本公式 + random(定力) 的凌潮心法點數，同時累積基本內功。 */
int do_exercise(object me)
{
    if( me->query_stat("kee") < 3 || me->query_stat("gin") < 3 ) {
        tell_object(me, "你覺得氣血浮動、精力難繼﹐已無法再運轉凌潮心法。\n");
        return 0;
    }

    me->consume_stat("gin", 2);
    me->damage_stat("gin", 1);
    me->consume_stat("kee", 2);
    me->damage_stat("kee", 1);

    me->improve_restored_force_tick();
    me->improve_skill_exact("tidy force",
        COMBAT_D->int_formula_exp(me)
        + random(me->query_attr("cps")));
    return 1;
}

int halt_exercise(object me, object owner, object from, string how)
{
    return 1;
}

/* refresh：扣 10 + 凌潮心法/10 的氣，回復 凌潮心法 × 3/2 的精；自身停頓 1 回合。 */
int do_refresh(object me)
{
    int sk, cost;

    sk = me->query_skill("tidy force", 1);
    cost = 10 + sk / 10;
    if( me->query_stat("kee") <= cost )
        return notify_fail("你的氣不夠﹐無法運起凌潮心法回復精神。\n");
    me->consume_stat("kee", cost);
    me->supplement_stat("gin", sk * 3 / 2);
    message_vision("$N運起凌潮心法﹐真氣如潮水般一波波湧上靈台﹐精神為之一振。\n", me);
    me->start_busy(1);
    return 1;
}

varargs int exert_function(object me, string func, object target)
{
    switch(func) {
    case "tidy force":
        message_vision(HIC "$N盤膝坐定﹐默運凌潮心法﹐丹田真氣一起一落﹐有如潮汐往復。\n" NOR, me);
        me->start_busy((: do_exercise, me :), (: halt_exercise, me :));
        return 1;
    case "refresh":
        return do_refresh(me);
    default:
        return notify_fail("凌潮心法沒有這種功能。\n");
    }
}
