/*---
description: 大邪心法（NEW）。哭笑門內功。練成 20 級；功能：修習、refresh、recover（噴血回氣）、
             armor（大邪金身）。設計見 docs/martial_arts/哭笑門_大邪心法與其他武功.md。
---*/
#include <ansi.h>
inherit SKILL;

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("huge force");
    setup();
}

int valid_enable(string usage)
{
    return usage == "force";
}

/* 學習條件：哭笑門弟子（人物等級 11 由師父把關）。 */
int valid_learn(object me)
{
    if( me->query("custom_faction") != "fighter.huge" )
        return notify_fail("你不是哭笑門弟子﹐大邪心法的路數你學不來。\n");
    return 1;
}

/*
 * 成長（使用者定案，2026-11-17）
 * 學成：20 級，根骨 +1。
 * 每級獎勵（20 級起，用升級後的等級）：精、氣上限各 +3；武術造詣 等級 × 10；
 *   41 級起武學之道 (等級 - 40) × 15。
 * 100 級：根骨 +1、膂力 +1（只給一次）。
 * growth_level 記錄已給過獎勵的等級，避免重複發放。
 */
int query_entry_level() { return 20; }

void skill_completed(object me, string sk)
{
    tell_object(me, HIY "你依法運轉數周天﹐只覺真氣霸道奔騰﹐終於練成了大邪心法。\n" NOR);
    if( !me->query("huge_force/initial_con_bonus") ) {
        me->set_attr("con", me->query_attr("con", 1) + 1);
        me->set("huge_force/initial_con_bonus", 1);
    }
}

void skill_advanced(object me, string sk)
{
    int lv;

    if( !userp(me) ) return;
    lv = me->query_skill("huge force", 1);
    if( lv < 20 ) return;
    if( me->query("huge_force/growth_level") >= lv ) return;
    me->set("huge_force/growth_level", lv);

    me->gain_score("martial art", lv * 10);
    if( lv > 40 ) me->gain_score("martial mastery", (lv - 40) * 15);
    me->advance_stat("gin", 3);
    me->advance_stat("kee", 3);

    if( lv >= 100 && !me->query("huge_force/bonus_100") ) {
        me->set("huge_force/bonus_100", 1);
        me->set_attr("con", me->query_attr("con", 1) + 1);
        me->set_attr("str", me->query_attr("str", 1) + 1);
        tell_object(me, HIY "你的大邪心法已臻大成﹐根骨與膂力都增長了。\n" NOR);
    }
}

/* 修習：每次運功 氣 -3（受傷 2）、精 -3（受傷 2）。 */
int do_exercise(object me)
{
    if( me->query_stat("kee") < 5 || me->query_stat("gin") < 5 ) {
        tell_object(me, "你覺得氣血浮動、精力難繼﹐已無法再運轉大邪心法。\n");
        return 0;
    }

    me->consume_stat("kee", 3);
    me->damage_stat("kee", 2);
    me->consume_stat("gin", 3);
    me->damage_stat("gin", 2);

    me->improve_restored_force_tick();
    me->improve_skill_exact("huge force",
        COMBAT_D->int_formula_exp(me)
        + random(me->query_attr("cps")));
    return 1;
}

int halt_exercise(object me, object owner, object from, string how)
{
    return 1;
}

/* refresh（與瘋虎功公式一致）：消耗 大邪心法/8 的氣，回復 機敏＋大邪心法 的精；自身停頓 1 回合。 */
int do_refresh(object me)
{
    int sk, cost;

    sk = me->query_skill("huge force", 1);
    if( sk < 1 )
        return notify_fail("你的大邪心法尚未練成﹐還無法運氣回精。\n");
    cost = sk / 8;
    if( me->query_stat("kee") <= cost )
        return notify_fail("你的氣不夠﹐無法運起大邪心法回復精神。\n");
    me->consume_stat("kee", cost);
    me->supplement_stat("gin", me->query_attr("dex") + sk);
    message_vision(BLU "$N深吸一口氣﹐精神為之一振。\n" NOR, me);
    me->start_busy(1);
    return 1;
}

/* recover：精的上限減少 15（受傷），恢復氣 大邪心法/2、氣的上限 大邪心法/6；自身停頓 2 回合。 */
int do_recover(object me)
{
    int sk;

    sk = me->query_skill("huge force", 1);
    if( sk < 1 )
        return notify_fail("你的大邪心法尚未練成﹐還無法運氣療傷。\n");
    if( me->query_stat("gin") <= 15 || me->query_stat_effective("gin") <= 15 )
        return notify_fail("你的精不夠﹐無法運起大邪心法療傷。\n");
    me->damage_stat("gin", 15);
    me->heal_stat("kee", sk / 6);
    me->supplement_stat("kee", sk / 2);
    message_vision(HIR "$N「哇」地噴出一大灘鮮血﹐蒼白的臉上恢復了一絲血色。\n" NOR, me);
    me->start_busy(2);
    return 1;
}

void remove_armor(object me, int bonus)
{
    if( !objectp(me) || !me->query_temp("huge_force/armor") ) return;
    me->add_temp("apply/armor", -bonus);
    me->delete_temp("huge_force/armor");
    tell_object(me, HIY "你體表的大邪金身漸漸散去。\n" NOR);
}

/* armor：大邪金身。大邪心法 90 級起；護甲值增加 大邪心法 點，持續 大邪心法 × 1.33 回合（一回合兩秒）。 */
int do_armor(object me)
{
    int sk, duration;

    sk = me->query_skill("huge force", 1);
    if( sk < 90 )
        return notify_fail("你的大邪心法火候未足﹐還無法施展大邪金身。\n");
    if( me->query_temp("huge_force/armor") )
        return notify_fail("你現在正處於大邪金身的護體狀態。\n");

    duration = sk * 4 / 3;
    me->add_temp("apply/armor", sk);
    me->set_temp("huge_force/armor", 1);
    message_vision(HIY "$N氣衝百穴﹐施展「大邪金身」﹐渾身上下鏘鏘作響﹐宛如金鐵交鳴！\n" NOR, me);
    call_out("remove_armor", duration * 2, me, sk);
    return 1;
}

varargs int exert_function(object me, string func, object target)
{
    switch(func) {
    case "huge force":
        message_vision(HIY "$N身若凌虛而超華嶽﹐氣如沖宵而撼北辰﹐開始修習大邪心法。\n" NOR, me);
        me->start_busy((: do_exercise, me :), (: halt_exercise, me :));
        return 1;
    case "refresh":
        return do_refresh(me);
    case "recover":
        return do_recover(me);
    case "armor":
        return do_armor(me);
    default:
        return notify_fail("大邪心法沒有這種功能。\n");
    }
}
