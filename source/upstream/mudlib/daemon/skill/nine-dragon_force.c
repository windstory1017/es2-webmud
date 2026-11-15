/*---
description: 九龍神功（NEW 新版武功寫法）。紫衣派內功，enable 在 force 上使用。
             exert nine-dragon force／heal／regenerate／overload。設計見 docs/martial_arts/紫衣派_九龍神功.md。
---*/
#include <ansi.h>

inherit "/std/martial_art";

#define FACTION   "taoist.thunder"
#define SKILL_ID  "nine-dragon force"

private void create()
{
    seteuid(getuid());

    art_id    = SKILL_ID;
    art_name  = "九龍神功";
    art_usage = "force";
    art_desc  = "紫衣派的內功心法，體內九道真氣如龍遊走，周天自成一體；"
                + "能以精化氣、化神，大成之後更能喚來雷光，助長天師道法的威力。";

    art_entry = 5;   // 練成 5 級（使用者指示，2026-11-14）

    ma_coef  = 10;
    mm_start = 30;
    mm_minus = 29;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon(SKILL_ID);
    setup();
}

/* 學習條件：人物等級 10、紫衣派弟子（使用者定案，2026-11-14）。 */
int valid_learn(object me)
{
    if( !is_faction(me, FACTION) )
        return notify_fail("你不是紫衣派弟子﹐無法修練九龍神功。\n");
    if( me->query_level() < 10 )
        return notify_fail("你的根基尚淺﹐還承受不住九龍神功的真氣。\n");
    return 1;
}

/* 內功不會拿來出招。 */
void attack_using(object me, object opponent, object weapon) { }

/* 學成（練成 5 級）。 */
void skill_completed(object me, string skill)
{
    tell_object(me, HIY "你體內九道真氣如龍遊走﹐周天運轉自成一體﹐九龍神功已有小成。\n" NOR);
}

/* 每級獎勵：超過學成等級後每升一級，精上限 +1～2、氣上限 +2、神上限 +1～3；
 * 等級 100 時根骨 +2、慧根 +1（只給一次，記在 nine_dragon/milestone）。 */
void skill_advanced(object me, string skill)
{
    int lv;

    ::skill_advanced(me, skill);

    if( !userp(me) ) return;
    lv = me->query_skill(SKILL_ID, 1);
    if( lv > 5 ) {
        me->advance_stat("gin", 1 + random(2));
        me->advance_stat("kee", 2);
        me->advance_stat("sen", 1 + random(3));
    }
    if( lv >= 100 && !me->query("nine_dragon/milestone") ) {
        me->set("nine_dragon/milestone", 1);
        me->set_attr("con", me->query_attr("con", 1) + 2);
        me->set_attr("wis", me->query_attr("wis", 1) + 1);
        tell_object(me, HIY "九道真氣在你體內圓轉如意﹐筋骨為之一壯﹐靈台也更加清明。\n"
            "你的根骨增加了。\n你的慧根增加了。\n" NOR);
    }
}

/* 修習：每回合消耗 精 2、氣 2、神 3（受傷減少上限 精 1、氣 1、神 2；使用者定案，2026-11-14），
 * 得到 基本公式 + random(慧根) 的九龍神功點數，同時累積基本內功。 */
int do_exercise(object me)
{
    if( me->query_stat("gin") < 3 || me->query_stat("kee") < 3 || me->query_stat("sen") < 4 ) {
        tell_object(me, "你只覺心神不寧、真氣散亂﹐再也運轉不動九龍真氣。\n");
        return 0;   // 傳回 0 結束打坐
    }

    me->consume_stat("gin", 2);
    me->damage_stat("gin", 1);
    me->consume_stat("kee", 2);
    me->damage_stat("kee", 1);
    me->consume_stat("sen", 3);
    me->damage_stat("sen", 2);

    me->improve_restored_force_tick();
    me->improve_skill_exact(SKILL_ID,
        COMBAT_D->int_formula_exp(me)
        + random(me->query_attr("wis")));
    return 1;
}

int halt_exercise(object me, object owner, object from, string how)
{
    return 1;
}

/* heal：消耗精的現在值 5 + 九龍神功/20，回復氣的現在值 10 + 九龍神功/2；自身停頓 1 tick。 */
int do_heal(object me)
{
    int sk, cost;

    sk = me->query_skill(SKILL_ID, 1);
    cost = 5 + sk / 20;
    if( me->query_stat("gin") <= cost )
        return notify_fail("你精神不濟﹐運不起九龍真氣。\n");

    me->consume_stat("gin", cost);
    me->supplement_stat("kee", 10 + sk / 2);
    message_vision("$N閉目調息﹐一縷真氣自精化入氣海﹐蒼白的臉色漸漸泛起血色。\n", me);
    me->start_busy(1);
    return 1;
}

/* regenerate：消耗精的現在值 5 + 九龍神功/10，回復神的現在值 10 + 九龍神功/2；自身停頓 1 tick。 */
int do_regenerate(object me)
{
    int sk, cost;

    sk = me->query_skill(SKILL_ID, 1);
    cost = 5 + sk / 10;
    if( me->query_stat("gin") <= cost )
        return notify_fail("你精神不濟﹐運不起九龍真氣。\n");

    me->consume_stat("gin", cost);
    me->supplement_stat("sen", 10 + sk / 2);
    message_vision("$N引一縷真氣上行﹐灌入靈台﹐眼中的倦意一掃而空。\n", me);
    me->start_busy(1);
    return 1;
}

/* overload：消耗神 75，喚來雷光；下次施展天師系咒的基礎傷害提昇 50%，沒用掉就一直保留到下次施咒
 * （天師道法 taoism-spells.c 已讀取 taoism/overload：威力 ×1.5，用掉就清除，六卦熒雷不消耗）；自身停頓 1 tick。 */
int do_overload(object me)
{
    if( me->query_temp("taoism/overload") )
        return notify_fail("雷光仍環繞在你周身﹐還沒有消散。\n");
    if( me->query_stat("sen") <= 75 )
        return notify_fail("你心神耗損過甚﹐喚不來雷光。\n");

    me->consume_stat("sen", 75);
    me->set_temp("taoism/overload", 1);
    message_vision(HIY "$N施咒喚來九道雷光如白鏈般環繞週身﹐不停的「嘶嘶」做響！\n" NOR, me);
    me->start_busy(1);
    return 1;
}

varargs int exert_function(object me, string func, object target)
{
    switch(func) {
    case "nine-dragon force":
        message_vision(HIY "$N盤膝坐下﹐五心向天﹐引九道真氣在體內緩緩運轉周天。\n" NOR, me);
        me->start_busy((: do_exercise, me :), (: halt_exercise, me :));
        return 1;
    case "heal":
        return do_heal(me);
    case "regenerate":
        return do_regenerate(me);
    case "overload":
        return do_overload(me);
    default:
        return notify_fail("九龍神功沒有這種功能。\n");
    }
}
