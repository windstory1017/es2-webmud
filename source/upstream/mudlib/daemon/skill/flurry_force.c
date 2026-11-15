/*---
description: 迴風拂柳心訣（NEW 新版武功寫法）。古劍門內功，enable 在 force 上使用。
             練成 10 級，練成時同時移除寒梅心法；之後每級精 +2、氣 +2、神 +1，
             20／100／160 級各有一次體質提升。設計見 docs/martial_arts/傲劍訣_景愈濤戰鬥敘述.md。
---*/
#include <ansi.h>

inherit "/std/martial_art";

private void create()
{
    seteuid(getuid());

    art_id    = "flurry force";
    art_name  = "迴風拂柳心訣";
    art_usage = "force";
    art_desc  = "古劍門的內功心訣，真氣如風拂柳、綿綿不絕，助長精氣神與體質。";

    art_entry = 10;

    ma_coef  = 10;     // 武術造詣 = 等級 × 10
    mm_start = 41;     // 武學之道 = (等級 - 40) × 10
    mm_minus = 40;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("flurry force");
    setup();
}

/* 學習條件：古劍門專屬。 */
int valid_learn(object me)
{
    if( !is_faction(me, "fighter.au-chan") )
        return notify_fail("你不是古劍門弟子﹐無法修練迴風拂柳心訣。\n");
    return 1;
}

/* 內功不會拿來出招。 */
void attack_using(object me, object opponent, object weapon) { }

/* 練成（10 級）：移除寒梅心法。 */
void skill_completed(object me, string skill)
{
    tell_object(me, HIY "你的迴風拂柳心訣已有小成了。\n" NOR);
    if( me->query_skill("hainmay force", 1) > 0 ) {
        me->delete_skill("hainmay force");
        tell_object(me, HIR "你體內聚集的寒梅真氣漸漸消散了。\n" NOR);
    }
}

/* 每級獎勵（引擎）；練成之後每升一級：精 +2、氣 +2、神 +1。
 * 20 級：膂力、膽識各 +1，神 +30；100 級：各 +2，精 +60；160 級：各 +3，氣 +90（各只給一次）。 */
void skill_advanced(object me, string skill)
{
    int lv;

    ::skill_advanced(me, skill);
    if( !userp(me) ) return;

    lv = me->query_skill("flurry force", 1);
    if( lv > 10 ) {
        me->advance_stat("gin", 2);
        me->advance_stat("kee", 2);
        me->advance_stat("sen", 1);
    }

    if( lv >= 20 && !me->query("flurry_force/bonus_20") ) {
        me->set("flurry_force/bonus_20", 1);
        me->set_attr("str", me->query_attr("str", 1) + 1);
        me->set_attr("cor", me->query_attr("cor", 1) + 1);
        me->advance_stat("sen", 30);
        tell_object(me, HIG "你終於打通了體內幾處要穴，體質更上一層樓了！\n" NOR
            HIY "你的神、膂力和膽識均有所提高。\n" NOR);
    }
    if( lv >= 100 && !me->query("flurry_force/bonus_100") ) {
        me->set("flurry_force/bonus_100", 1);
        me->set_attr("str", me->query_attr("str", 1) + 2);
        me->set_attr("cor", me->query_attr("cor", 1) + 2);
        me->advance_stat("gin", 60);
        tell_object(me, HIY "你打通了體內數道筋脈，體質已近頂峰！\n"
            "你的精、膂力和膽識均有所提高。\n" NOR);
    }
    if( lv >= 160 && !me->query("flurry_force/bonus_160") ) {
        me->set("flurry_force/bonus_160", 1);
        me->set_attr("str", me->query_attr("str", 1) + 3);
        me->set_attr("cor", me->query_attr("cor", 1) + 3);
        me->advance_stat("kee", 90);
        tell_object(me, HIY "你週身經脈穴位渾然一體，體質已是無人能比！\n"
            "你的氣、膂力和膽識均有所提高。\n" NOR);
    }
}

/* 修習（exert flurry force）：每次運功消耗氣、精、神各 2（受傷各 1），
 * 得到 基本公式 + random(定力) 的點數，同時累積基本內功。 */
int do_exercise(object me)
{
    if( me->query_stat("kee") < 3 || me->query_stat("gin") < 3 || me->query_stat("sen") < 3 ) {
        tell_object(me, "你覺得氣血浮動、精神不濟﹐已無法再運轉迴風拂柳心訣。\n");
        return 0;
    }

    me->consume_stat("kee", 2);
    me->damage_stat("kee", 1);
    me->consume_stat("gin", 2);
    me->damage_stat("gin", 1);
    me->consume_stat("sen", 2);
    me->damage_stat("sen", 1);

    me->improve_restored_force_tick();
    me->improve_skill_exact("flurry force",
        COMBAT_D->int_formula_exp(me)
        + random(me->query_attr("cps")));
    return 1;
}

int halt_exercise(object me, object owner, object from, string how)
{
    return 1;
}

/* refresh（戰鬥中可用）：自身停頓 1 回合；消耗 5 + 等級/10 的氣，回復 等級 點精。 */
int do_refresh(object me)
{
    int sk, cost;

    sk = me->query_skill("flurry force", 1);
    cost = 5 + sk / 10;
    if( me->query_stat("kee") <= cost )
        return notify_fail("你的氣不夠﹐無法運起迴風拂柳心訣回復精神。\n");
    me->consume_stat("kee", cost);
    me->supplement_stat("gin", sk);
    message_vision("$N運起迴風拂柳心訣﹐真氣如風拂過靈台﹐精神為之一振。\n", me);
    me->start_busy(1);
    return 1;
}

/* heal（戰鬥中可用）：自身停頓 2 回合；消耗 5 + 等級/20 的精，回復 等級/2 的氣，並回復 等級/4 的氣上限（受傷減少的部分）。 */
int do_heal(object me)
{
    int sk, cost;

    sk = me->query_skill("flurry force", 1);
    cost = 5 + sk / 20;
    if( me->query_stat("gin") <= cost )
        return notify_fail("你的精不夠﹐無法運起迴風拂柳心訣療傷。\n");
    me->consume_stat("gin", cost);
    me->heal_stat("kee", sk / 4);
    me->supplement_stat("kee", sk / 2);
    message_vision("$N運起迴風拂柳心訣調息療傷﹐臉色漸漸好轉。\n", me);
    me->start_busy(2);
    return 1;
}

varargs int exert_function(object me, string func, object target)
{
    switch(func) {
    case "flurry force":
        message_vision(HIY "$N盤膝坐定﹐默運迴風拂柳心訣﹐真氣如風拂柳在週身經脈流轉。\n" NOR, me);
        me->start_busy((: do_exercise, me :), (: halt_exercise, me :));
        return 1;
    case "refresh":
        return do_refresh(me);
    case "heal":
        return do_heal(me);
    default:
        return notify_fail("迴風拂柳心訣沒有這種功能。\n");
    }
}
