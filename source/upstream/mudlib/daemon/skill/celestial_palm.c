/*---
description: 天邪神掌（NEW 新版武功寫法）。雪吟莊以天邪虎的形象研習出的掌法，enable 在 unarmed。
             出掌後有機會定人、發出虎嘯、回氣（機率看自己的行動力）；
             絕招 perform unarmed:cele zarn（天邪掌）。設計見 docs/martial_arts/雪吟莊_武功.md。
---*/
#include <ansi.h>

inherit "/std/martial_art";

#define FACTION "fighter.snow"

private void create()
{
    seteuid(getuid());

    art_id    = "celestial palm";
    art_name  = "天邪神掌";
    art_usage = "unarmed";
    art_desc  = "雪吟莊以天邪虎的形象研習出的掌法，撲、剪、掀、嘯皆有虎形；"
                + "掌力中夾著天邪真氣，能震懾對手、嘯傷群敵。";
    art_bonus = 0;

    moves = ({
        ([ "name": "猛虎出柙", "attack": 8, "defense": 3, "force": 2, "bonus": 5, "damage_type": "瘀傷",
           "action": "$N雙掌一錯﹐使出「猛虎出柙」﹐右掌直拍$n的$l" ]),
        ([ "name": "餓虎撲羊", "attack": 9, "defense": 2, "force": 2, "bonus": 5, "damage_type": "瘀傷",
           "action": "$N矮身前撲﹐左掌按地借勢﹐「餓虎撲羊」自下印向$n的$l" ]),
        ([ "name": "虎尾翻雲", "attack": 7, "defense": 4, "force": 2, "bonus": 5, "damage_type": "瘀傷",
           "action": "$N擰腰轉身﹐一記「虎尾翻雲」反手劈向$n的$l" ]),
        ([ "name": "伏虎聽風", "attack": 7, "defense": 5, "force": 2, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N伏身斂息似在聽風﹐忽然雙掌齊推﹐按向$n的$l" ]),
        ([ "name": "虎爪穿雲", "attack": 9, "defense": 3, "force": 2, "bonus": 10, "damage_type": "抓傷",
           "action": "$N五指半屈如鉤﹐「虎爪穿雲」自$n肘底穿入﹐切向$n的$l" ]),
        ([ "name": "天邪嘯月", "attack": 8, "defense": 3, "force": 3, "bonus": 15, "damage_type": "內傷",
           "action": "$N仰首一聲長嘯﹐掌風隨嘯聲而至﹐「天邪嘯月」推向$n的$l" ]),
    });

    art_entry_msg = "你練成了天邪神掌﹐掌勢起落之間﹐隱隱已有猛虎下山的威勢。\n";

    ma_coef  = 10;
    mm_start = 30;
    mm_minus = 29;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("celestial palm");
    setup();
}

/* 學習條件：天邪神功 180。雪吟莊弟子不會天邪神掌（使用者指示，2026-11-06）；之後由解謎習得（尚未製作）。 */
int valid_learn(object me)
{
    if( me->query_skill("celestial force", 1) < 180 )
        return notify_fail("你的天邪神功火候未到﹐還使不出天邪虎的掌勢。\n");
    return 1;
}

/* 依自己的行動力成長的機率：base% + 行動力/div，最高 max%。 */
int move_chance(object me, int base, int div, int max)
{
    int c = base + me->query_ability("move") / div;
    return c > max ? max : c;
}

/* 出掌後的特效（機率都看自己的行動力，行動力 300 時到上限）：
 *   定人：打中後，對手停頓 2 回合；10% + 行動力/30，最高 20%。
 *   虎嘯：全體對手受 天邪神掌/5 + 根骨 的傷害；10% + 行動力/20，最高 25%。
 *   回氣：自身回復 天邪神掌/6 的氣；10% + 行動力/30，最高 20%。 */
void after_strike(object me, object opponent, object weapon, mapping m, int damage)
{
    object ob, *enemies;
    int sk, dmg;

    if( !objectp(opponent) || !living(me) ) return;
    sk = me->query_skill("celestial palm", 1);

    if( damage > 0 && living(opponent) && environment(opponent) == environment(me)
    &&  !opponent->is_busy() && random(100) < move_chance(me, 10, 30, 20) ) {
        // 一行、前後各空一行（使用者指示，2026-11-07）
        message_vision(HIY "\n$N掌勢一沉﹐一股天邪真氣透掌而入﹐$n只覺週身一麻﹐竟動彈不得！\n\n" NOR,
            me, opponent, 1);
        opponent->start_busy(2);
    }

    if( random(100) < move_chance(me, 10, 20, 25) ) {
        enemies = ({});
        foreach( ob in me->query_enemy() )
            if( objectp(ob) && living(ob) && environment(ob) == environment(me) )
                enemies += ({ ob });
        if( sizeof(enemies) ) {
            message_vision(HIR "\n$N猛地仰天一聲虎嘯﹐嘯聲中夾著天邪真氣﹐震得四周敵人氣血翻湧！\n\n" NOR, me, 0, 1);   // 一行、前後空一行（2026-11-07）
            dmg = sk / 5 + me->query_attr("con");
            foreach( ob in enemies ) {
                if( !objectp(ob) ) continue;
                ob->receive_damage(dmg, me, me);
                if( objectp(ob) ) show_status(ob, kee_ratio(ob));
            }
        }
    }

    if( random(100) < move_chance(me, 10, 30, 20) ) {
        message_vision(HIG "$N掌勢一收一吐之間﹐暗暗調勻了內息﹐臉色好看了些。\n" NOR, me);
        me->supplement_stat("kee", sk / 6);
    }
}

void remove_cele_zarn(object me, int amount)
{
    if( !objectp(me) || !me->query_temp("celestial_palm/zarn") ) return;
    me->add_temp("apply/armor", -amount);
    me->add_temp("apply/intimidate", -amount);
    me->add_temp("apply/wittiness", -amount);
    me->delete_temp("celestial_palm/zarn");
    tell_object(me, HIY "你周身的天邪真氣漸漸散去﹐收起了天邪掌的架勢。\n" NOR);
}

/* 絕招 cele zarn（天邪掌）：扣 100 點目前的氣；護甲、攻勢等級、守勢等級各 +天邪神掌 × 2/5；
 * 持續 天邪神掌/5 回合；效果中不能再用。 */
int perform_action(object me, string act, object target)
{
    int sk, amount, duration;

    if( act != "cele zarn" ) return notify_fail("天邪神掌沒有這種絕招。\n");
    if( me->query_temp("celestial_palm/zarn") )
        return notify_fail("你已經擺出天邪掌的架勢了。\n");
    if( me->query_stat("kee") <= 100 )
        return notify_fail("你的氣不夠﹐使不出天邪掌。\n");

    sk = me->query_skill("celestial palm", 1);
    amount = sk * 2 / 5;
    duration = sk / 5;
    if( duration < 1 ) duration = 1;

    me->consume_stat("kee", 100);
    me->add_temp("apply/armor", amount);
    me->add_temp("apply/intimidate", amount);
    me->add_temp("apply/wittiness", amount);
    me->set_temp("celestial_palm/zarn", 1);
    message_vision(HIY "$N催動體內天邪氣勁，雙掌交錯將真氣自掌心流遍全身！\n" NOR, me, 0, 1);   // 使用者指定敘述（2026-11-07）
    call_out("remove_cele_zarn", duration * 2, me, amount);
    return 1;
}
