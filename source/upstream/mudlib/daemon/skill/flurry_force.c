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

/* 修習：每次運功消耗氣 2（受傷 1）、精 3（受傷 1），
 * 得到 基本公式 + random(定力) 的點數，同時累積基本內功（與寒梅心法相同）。 */
int do_exercise(object me)
{
    if( me->query_stat("kee") < 3 || me->query_stat("gin") < 4 ) {
        tell_object(me, "你覺得氣血浮動、精力難繼﹐已無法再運轉迴風拂柳心訣。\n");
        return 0;
    }

    me->consume_stat("kee", 2);
    me->damage_stat("kee", 1);
    me->consume_stat("gin", 3);
    me->damage_stat("gin", 1);

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
