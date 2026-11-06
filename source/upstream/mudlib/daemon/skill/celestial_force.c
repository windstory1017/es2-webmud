/*---
description: 天邪神功（NEW 新版武功寫法）。雪吟莊源自天邪國的內功，enable 在 force 上使用。
             exert celestial force／heal／roar（天邪虎嘯）／ghost。設計見 docs/martial_arts/雪吟莊_武功.md。
---*/
#include <ansi.h>

inherit "/std/martial_art";

#define FACTION "fighter.snow"

private void create()
{
    seteuid(getuid());

    art_id    = "celestial force";
    art_name  = "天邪神功";
    art_usage = "force";
    art_desc  = "源自天邪國的霸道內功，雪吟莊憑此立足江湖。可運氣療傷、"
                + "發出震懾群敵的天邪虎嘯，或凝神開啟陰陽眼。";

    art_entry = 20;

    ma_coef  = 10;
    mm_start = 30;
    mm_minus = 29;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("celestial force");
    setup();
}

/* 學習條件：雪吟莊弟子、人物等級 10、凌潮心法練到個人上限（天賦值）。 */
int valid_learn(object me)
{
    int lv;

    if( !is_faction(me, FACTION) )
        return notify_fail("你不是雪吟莊弟子﹐無法修練天邪神功。\n");
    if( me->query_level() < 10 )
        return notify_fail("你的根基尚淺﹐還承受不住天邪神功的霸道真氣。\n");
    lv = me->query_skill("tidy force", 1);
    if( lv < 1 || lv < me->query_personal_skill_cap("tidy force") )
        return notify_fail("你的凌潮心法尚未練到極處﹐還無法轉修天邪神功。\n");
    return 1;
}

/* 內功不會拿來出招。 */
void attack_using(object me, object opponent, object weapon) { }

/* 學成：累積到 20 級門檻，gain 時直接練成 20 級。 */
void skill_completed(object me, string skill)
{
    tell_object(me, HIY "你練成了天邪神功﹐一股霸道的真氣在丹田中盤旋﹐隱隱有虎嘯之聲。\n" NOR);
}

/* 每級獎勵（引擎）；練成之後每升一級：精上限 +1～3、氣上限 +3、神上限 +0～2。 */
void skill_advanced(object me, string skill)
{
    ::skill_advanced(me, skill);

    if( userp(me) && me->query_skill("celestial force", 1) > 20 ) {
        me->advance_stat("gin", 1 + random(3));
        me->advance_stat("kee", 3);
        me->advance_stat("sen", random(3));
    }
}

/* 修習：每次運功消耗目前的精 2、氣 2，精、氣上限各受傷 1；
 * 得到 基本公式 + random(定力) 的天邪神功點數，同時累積基本內功。 */
int do_exercise(object me)
{
    if( me->query_stat("kee") < 3 || me->query_stat("gin") < 3 ) {
        tell_object(me, "你覺得氣血浮動、精力難繼﹐已無法再運轉天邪神功。\n");
        return 0;
    }

    me->consume_stat("gin", 2);
    me->damage_stat("gin", 1);
    me->consume_stat("kee", 2);
    me->damage_stat("kee", 1);

    me->improve_restored_force_tick();
    me->improve_skill_exact("celestial force",
        COMBAT_D->int_formula_exp(me)
        + random(me->query_attr("cps")));
    return 1;
}

int halt_exercise(object me, object owner, object from, string how)
{
    return 1;
}

/* heal：扣 天邪神功/20 的神，回復 天邪神功/2 + 根骨 的氣，並回復同量受傷的氣上限。 */
int do_heal(object me)
{
    int sk, cost, n;

    sk = me->query_skill("celestial force", 1);
    cost = sk / 20;
    if( me->query_stat("sen") <= cost )
        return notify_fail("你的神不夠﹐無法運起天邪神功療傷。\n");
    n = sk / 2 + me->query_attr("con");
    me->consume_stat("sen", cost);
    me->heal_stat("kee", n);
    me->supplement_stat("kee", n);
    message_vision("$N運起天邪神功﹐一股熾熱的真氣在週身遊走﹐傷勢漸漸好轉。\n", me);
    return 1;
}

/* 天邪虎嘯：扣 天邪神功/20 的神；正在和自己交手的對手全部停頓 2 回合，
 * 受到 天邪神功/5 + 根骨 的傷害；自身回復 天邪神功/2 的氣，用完停頓 1 回合（使用者指示，2026-11-06）。
 * 不在戰鬥中的人不受影響。 */
int do_roar(object me)
{
    object *enemies, ob;
    int sk, cost, dmg;

    enemies = ({});
    foreach( ob in me->query_enemy() )
        if( objectp(ob) && living(ob) && environment(ob) == environment(me) )
            enemies += ({ ob });
    if( !sizeof(enemies) )
        return notify_fail("你並未與人交手﹐無須發出天邪虎嘯。\n");

    sk = me->query_skill("celestial force", 1);
    cost = sk / 20;
    if( me->query_stat("sen") <= cost )
        return notify_fail("你的神不夠﹐無法發出天邪虎嘯。\n");
    me->consume_stat("sen", cost);

    message_vision(HIY "\n$N深吸一口氣﹐運起天邪神功仰天長嘯﹐嘯聲有如猛虎出山﹐震得四壁嗡嗡作響！\n\n" NOR, me, 0, 1);   // 整句一行、不自動斷行（使用者指示，2026-11-06）
    dmg = sk / 5 + me->query_attr("con");
    foreach( ob in enemies ) {
        if( !objectp(ob) ) continue;
        message_vision(HIY "$n被嘯聲震得耳中嗡嗡作響﹐一時呆立當場。\n" NOR, me, ob);
        ob->start_busy(2);
        ob->receive_damage(dmg, me, me);
        if( objectp(ob) ) show_status(ob, kee_ratio(ob));
    }
    me->supplement_stat("kee", sk / 2);
    me->start_busy(1);
    return 1;
}

void remove_ghost(object me, int aware, int intimidate)
{
    if( !objectp(me) || !me->query_temp("celestial/ghost") ) return;
    me->add_temp("apply/awarness", -aware);
    me->add_temp("apply/intimidate", -intimidate);
    me->add_temp("apply/vision_of_ghost", -1);
    me->delete_temp("celestial/ghost");
    tell_object(me, CYN "你眼中的幽光漸漸散去﹐四周又恢復了平常的模樣。\n" NOR);
}

/* ghost：警覺 +天邪神功/2、攻勢等級 +天邪神功/4、陰陽眼；持續 天邪神功/5 回合。
 * 效果中不能重複使用。 */
int do_ghost(object me)
{
    int sk, aware, intimidate, duration;

    if( me->query_temp("celestial/ghost") )
        return notify_fail("你的陰陽眼已經開著了。\n");

    sk = me->query_skill("celestial force", 1);
    aware = sk / 2;
    intimidate = sk / 4;
    duration = sk / 5;
    if( duration < 1 ) duration = 1;

    me->add_temp("apply/awarness", aware);
    me->add_temp("apply/intimidate", intimidate);
    me->add_temp("apply/vision_of_ghost", 1);
    me->set_temp("celestial/ghost", 1);
    message_vision(HIB "$N運起天邪神功凝聚雙目﹐眼中泛起一層幽幽的青光。\n" NOR, me);
    call_out("remove_ghost", duration * 2, me, aware, intimidate);
    return 1;
}

varargs int exert_function(object me, string func, object target)
{
    switch(func) {
    case "celestial force":
        message_vision(HIY "$N盤膝坐定﹐默運天邪神功﹐丹田中一股霸道的真氣緩緩流轉。\n" NOR, me);
        me->start_busy((: do_exercise, me :), (: halt_exercise, me :));
        return 1;
    case "heal":
        return do_heal(me);
    case "roar":
        return do_roar(me);
    case "ghost":
        return do_ghost(me);
    default:
        return notify_fail("天邪神功沒有這種功能。\n");
    }
}
