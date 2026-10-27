/*---
description: 寒梅心法（NEW 新版武功寫法）。冷梅莊內功，enable 在 force 上使用。
             exert hainmay force／refresh／recover／mirror。設計見 docs/martial_arts/冷梅莊_劍士武功.md。
---*/
#include <ansi.h>

inherit "/std/martial_art";

private void create()
{
    seteuid(getuid());

    art_id    = "hainmay force";
    art_name  = "寒梅心法";
    art_usage = "force";
    art_desc  = "冷梅莊的內功心法，可運氣回精、療傷，心法大成之後能進入【 明 鏡 止 水 】之境。";

    art_entry = 10;

    ma_coef  = 10;
    mm_start = 30;
    mm_minus = 29;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("hainmay force");
    setup();
}

/* 學習條件：等級 15、冷梅莊弟子。 */
int valid_learn(object me)
{
    if( !is_faction(me, "fighter.lunmay") )
        return notify_fail("你不是冷梅莊弟子﹐無法修練寒梅心法。\n");
    if( me->query_level() < 15 )
        return notify_fail("你的根基尚淺﹐還承受不住寒梅心法的清冷真氣。\n");
    return 1;
}

/* 內功不會拿來出招。 */
void attack_using(object me, object opponent, object weapon) { }

/* 學成：累積到 10 級門檻，gain 時直接練成 10 級（第一重）。 */
void skill_completed(object me, string skill)
{
    tell_object(me, HIW "你練成寒梅心法第一重﹗\n" NOR);
}

/* 每級獎勵（引擎）；11 級起每級精 +2～3、氣 +2、神 +0～1；每 10 級練成一重（20 級第二重……100 級第十重）。
 * 100 級：修完十重基本心法，膂力 +1、膽識 +1、精上限 +100、氣上限 +100。 */
void skill_advanced(object me, string skill)
{
    int lv;

    ::skill_advanced(me, skill);

    lv = me->query_skill("hainmay force", 1);
    // 練成（10 級）之後每升一級：精 +2～3、氣 +2、神 +0～1（使用者定案，2026-10-23）。
    if( userp(me) && lv > 10 ) {
        me->advance_stat("gin", 2 + random(2));
        me->advance_stat("kee", 2);
        me->advance_stat("sen", random(2));
    }
    if( lv >= 20 && lv <= 100 && lv % 10 == 0 )
        tell_object(me, HIW "你練成了寒梅心法第" + chinese_number(lv / 10) + "重。\n" NOR);

    if( lv >= 100 && !me->query("hainmay/bonus_100") ) {
        me->set("hainmay/bonus_100", 1);
        me->set_attr("str", me->query_attr("str", 1) + 1);
        me->set_attr("cor", me->query_attr("cor", 1) + 1);
        me->advance_stat("gin", 100);
        me->advance_stat("kee", 100);
        tell_object(me, HIW "你修煉寒梅心法有成，已經修完了十重基本心法。\n"
            "你的膂力增加了。\n"
            "你的膽識增加了。\n"
            "你的精、氣因為長年修煉寒梅心法增進不少。\n" NOR);
    }
}

/* 修習：每次運功消耗氣 2（受傷 1）、精 3（受傷 1），
 * 得到 基本公式 + random(定力) 的寒梅心法點數，同時累積基本內功。 */
int do_exercise(object me)
{
    if( me->query_stat("kee") < 3 || me->query_stat("gin") < 4 ) {
        tell_object(me, "你覺得氣血浮動、精力難繼﹐已無法再運轉寒梅心法。\n");
        return 0;   // 傳回 0 結束打坐（系統沒有 interrupt_me，舊寫法會一直重複這句；2026-10-23）
    }

    me->consume_stat("kee", 2);
    me->damage_stat("kee", 1);
    me->consume_stat("gin", 3);
    me->damage_stat("gin", 1);

    me->improve_restored_force_tick();
    me->improve_skill_exact("hainmay force",
        COMBAT_D->int_formula_exp(me)
        + random(me->query_attr("cps")));
    return 1;
}

int halt_exercise(object me, object owner, object from, string how)
{
    return 1;
}

/* refresh：消耗 寒梅心法/3 的氣，回復 寒梅心法 的精；自身停頓 1 回合。 */
int do_refresh(object me)
{
    int sk, cost;

    sk = me->query_skill("hainmay force", 1);
    cost = sk / 3;
    if( me->query_stat("kee") <= cost )
        return notify_fail("你的氣不夠﹐無法運起寒梅心法回復精神。\n");
    me->consume_stat("kee", cost);
    me->supplement_stat("gin", sk);
    message_vision("$N運起寒梅心法﹐一股清冷真氣直透靈台﹐精神為之一振。\n", me);
    me->start_busy(1);
    return 1;
}

/* recover：消耗 寒梅心法/10 的精，回復 10+寒梅心法/8 的氣上限（受傷減少的部分）
 * 與 10+寒梅心法/5 的氣；自身停頓 2 回合。 */
int do_recover(object me)
{
    int sk, cost;

    sk = me->query_skill("hainmay force", 1);
    cost = sk / 10;
    if( me->query_stat("gin") <= cost )
        return notify_fail("你的精不夠﹐無法運起寒梅心法療傷。\n");
    me->consume_stat("gin", cost);
    me->heal_stat("kee", 10 + sk / 8);
    me->supplement_stat("kee", 10 + sk / 5);
    message_vision("$N運起寒梅心法調息療傷﹐臉色漸漸好轉。\n", me);
    me->start_busy(2);
    return 1;
}

void remove_mirror(object me, int intimidate, int attack)
{
    if( !objectp(me) || !me->query_temp("hainmay/mirror") ) return;
    me->add_temp("apply/intimidate", -intimidate);
    me->add_temp("apply/attack", -attack);
    me->delete_temp("hainmay/mirror");
    tell_object(me, HIY "你的心神漸漸從明鏡止水之境中退了出來。\n" NOR);
}

/* mirror：寒梅心法 100 級起；扣 100 點目前的氣；效果中不能再用；沒有冷卻。
 * 攻勢等級 +寒梅心法/4、攻擊能力值 +寒梅心法/2；
 * 持續（定力×2 + 機敏）+ 寒梅心法/5 回合（屬性含裝備，一回合兩秒）。 */
int do_mirror(object me)
{
    int sk, intimidate, attack, duration;

    sk = me->query_skill("hainmay force", 1);
    if( sk < 100 )
        return notify_fail("你的寒梅心法尚未大成，無法進入明鏡止水之境。\n");
    if( me->query_temp("hainmay/mirror") )
        return notify_fail("你已經處於明鏡止水之境了。\n");
    if( me->query_stat("kee") <= 100 )
        return notify_fail("你的氣不夠﹐無法進入明鏡止水之境。\n");

    me->consume_stat("kee", 100);
    intimidate = sk / 4;
    attack = sk / 2;
    duration = me->query_attr("cps") * 2 + me->query_attr("dex") + sk / 5;

    me->add_temp("apply/intimidate", intimidate);
    me->add_temp("apply/attack", attack);
    me->set_temp("hainmay/mirror", 1);
    message_vision(HIW "$N心靜若水眼明如鏡﹐竟漸入【 明 鏡 止 水 】之境界！\n" NOR, me);
    call_out("remove_mirror", duration * 2, me, intimidate, attack);
    return 1;
}

varargs int exert_function(object me, string func, object target)
{
    switch(func) {
    case "hainmay force":
        message_vision(HIY "$N盤膝坐定﹐默運寒梅心法﹐一股清冷之氣在週身經脈流轉。\n" NOR, me);
        me->start_busy((: do_exercise, me :), (: halt_exercise, me :));
        return 1;
    case "refresh":
        return do_refresh(me);
    case "recover":
        return do_recover(me);
    case "mirror":
        return do_mirror(me);
    default:
        return notify_fail("寒梅心法沒有這種功能。\n");
    }
}
