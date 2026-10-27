/*---
description: 封山派內功（NEW 新版武功寫法）。封山派內功，enable 在 force 上使用。
             exert fonxan force／refresh／heal／focus。設計見 docs/martial_arts/封山派_封山劍法與倒亂七星步法.md。
---*/
#include <ansi.h>

inherit "/std/martial_art";

private void create()
{
    seteuid(getuid());

    art_id    = "fonxan force";
    art_name  = "封山派內功";
    art_usage = "force";
    art_desc  = "封山派的正宗心法，綿長醇厚，能運氣回精、療傷，"
                + "內力大成之後更能凝神專注，攻守俱增。";

    art_entry = 20;

    ma_coef  = 10;
    mm_start = 30;
    mm_minus = 29;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("fonxan force");
    setup();
}

/* 學習條件：等級 15、封山派弟子。 */
int valid_learn(object me)
{
    if( !is_faction(me, "fighter.fonxan") )
        return notify_fail("你不是封山派弟子﹐無法修練封山派內功。\n");
    if( me->query_level() < 15 )
        return notify_fail("你的根基尚淺﹐還承受不住封山派內功的醇厚真氣。\n");
    return 1;
}

/* 內功不會拿來出招。 */
void attack_using(object me, object opponent, object weapon) { }

/* 學成：累積到 20 級門檻，gain 時直接練成 20 級。 */
void skill_completed(object me, string skill)
{
    tell_object(me, HIW "你的封山派內功已有小成了。\n" NOR);
}

/* 每級獎勵（引擎）；超過學成等級後每升一級精、氣上限各 +2。
 * 100 級：膂力 +1、定力 +1（只給一次）。 */
void skill_advanced(object me, string skill)
{
    int lv;

    ::skill_advanced(me, skill);

    if( !userp(me) ) return;
    lv = me->query_skill("fonxan force", 1);
    if( lv > 20 ) {
        me->advance_stat("gin", 2);
        me->advance_stat("kee", 2);
    }

    if( lv >= 100 && !me->query("fonxan/bonus_100") ) {
        me->set("fonxan/bonus_100", 1);
        me->set_attr("str", me->query_attr("str", 1) + 1);
        me->set_attr("cps", me->query_attr("cps", 1) + 1);
        tell_object(me, HIW "你體內的封山派內力已經貫通了多處經脈﹐讓你的武功更上一層樓了！\n" NOR);
    }
}

/* 修習：每回合消耗精、氣各 2（受傷減少上限各 1），
 * 得到 基本公式 + random(定力) 的封山派內功點數，同時累積基本內功。 */
int do_exercise(object me)
{
    if( me->query_stat("kee") < 3 || me->query_stat("gin") < 3 ) {
        tell_object(me, "你覺得氣血浮動、精力難繼﹐已無法再運轉封山派內功。\n");
        return 0;   // 傳回 0 結束打坐（系統沒有 interrupt_me，舊寫法會一直重複這句；2026-10-23）
    }

    me->consume_stat("kee", 2);
    me->damage_stat("kee", 1);
    me->consume_stat("gin", 2);
    me->damage_stat("gin", 1);

    me->improve_restored_force_tick();
    me->improve_skill_exact("fonxan force",
        COMBAT_D->int_formula_exp(me)
        + random(me->query_attr("cps")));
    return 1;
}

int halt_exercise(object me, object owner, object from, string how)
{
    return 1;
}

/* refresh：消耗 封山派內功/20 的氣，回復 封山派內功/2 + random(機敏) + random(定力) 的精；
 * 自身停頓 1 回合。 */
int do_refresh(object me)
{
    int sk, cost, dex, cps;

    sk = me->query_skill("fonxan force", 1);
    cost = sk / 20;
    if( me->query_stat("kee") <= cost )
        return notify_fail("你的氣不夠﹐無法運起封山派內功回復精神。\n");
    dex = me->query_attr("dex");
    cps = me->query_attr("cps");
    me->consume_stat("kee", cost);
    me->supplement_stat("gin", sk / 2 + (dex > 0 ? random(dex) : 0) + (cps > 0 ? random(cps) : 0));
    message_vision("$N運起封山派內功﹐一股醇厚真氣流轉週身﹐精神為之一振。\n", me);
    me->start_busy(1);
    return 1;
}

/* heal：消耗 封山派內功/10 的精，回復 5+封山派內功/10 的氣上限（受傷減少的部分）
 * 與 10+封山派內功/5 的氣；自身停頓 2 回合。 */
int do_heal(object me)
{
    int sk, cost;

    sk = me->query_skill("fonxan force", 1);
    cost = sk / 10;
    if( me->query_stat("gin") <= cost )
        return notify_fail("你的精不夠﹐無法運起封山派內功療傷。\n");
    me->consume_stat("gin", cost);
    me->heal_stat("kee", 5 + sk / 10);
    me->supplement_stat("kee", 10 + sk / 5);
    message_vision("$N運起封山派內功調息療傷﹐臉色漸漸好轉。\n", me);
    me->start_busy(2);
    return 1;
}

void remove_focus(object me, int intimidate, int defense, int damage)
{
    if( !objectp(me) || !me->query_temp("fonxan/focus") ) return;
    me->add_temp("apply/intimidate", -intimidate);
    me->add_temp("apply/defense", -defense);
    me->add_temp("apply/damage", -damage);
    me->delete_temp("fonxan/focus");
    tell_object(me, HIY "你緊繃的心神漸漸鬆懈。\n" NOR);
}

/* focus：封山派內功 100 級起；扣 50 點目前的氣；效果中不能再用；戰鬥中可用。
 * 攻勢等級 +封山派內功/4、防禦能力值 +(封山派內功/4 ～ 封山派內功/2)、傷害力 +封山派內功/20；
 * 持續（定力×2 + 機敏）+ 封山派內功/5 回合（屬性含裝備，一回合兩秒）。 */
int do_focus(object me)
{
    int sk, intimidate, defense, damage, duration;

    sk = me->query_skill("fonxan force", 1);
    if( sk < 100 )
        return notify_fail("你的封山派內功未到火候。\n");
    if( me->query_temp("fonxan/focus") )
        return notify_fail("你已經凝神專注了。\n");
    if( me->query_stat("kee") <= 50 )
        return notify_fail("你的氣不夠﹐無法凝神專注。\n");

    me->consume_stat("kee", 50);
    intimidate = sk / 4;
    defense = sk / 4 + random(sk / 4 + 1);
    damage = sk / 20;
    duration = me->query_attr("cps") * 2 + me->query_attr("dex") + sk / 5;

    me->add_temp("apply/intimidate", intimidate);
    me->add_temp("apply/defense", defense);
    me->add_temp("apply/damage", damage);
    me->set_temp("fonxan/focus", 1);
    message_vision(HIW "$N凝神靜氣﹐將封山派內力運至週身﹐心神為之一凝。\n" NOR, me);
    call_out("remove_focus", duration * 2, me, intimidate, defense, damage);
    return 1;
}

varargs int exert_function(object me, string func, object target)
{
    switch(func) {
    case "fonxan force":
        message_vision(HIY "$N盤膝坐定﹐默運封山派內功﹐一股醇厚真氣在週身經脈緩緩流轉。\n" NOR, me);
        me->start_busy((: do_exercise, me :), (: halt_exercise, me :));
        return 1;
    case "refresh":
        return do_refresh(me);
    case "heal":
        return do_heal(me);
    case "focus":
        return do_focus(me);
    default:
        return notify_fail("封山派內功沒有這種功能。\n");
    }
}
