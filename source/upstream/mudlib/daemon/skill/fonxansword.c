/*---
description: 封山劍法（NEW 新版武功寫法）。封山派劍法，可 enable 在劍術與雙手劍，不能 enable 在招架上。
             特殊招式已製作但先不開放（SPECIALS_OPEN），柳東蘆例外；越鬥越長已開放。
             攻守兼備、變化多端，爆發不足；招式敘述取自「梅影風 vs 柳東蘆」對戰紀錄。
             設計見 docs/martial_arts/封山派_封山劍法與倒亂七星步法.md。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

/* 特殊招式（搶攻、八荒、封字訣、無形劍氣、上弦月／下弦月）已製作但先不開放
 * （使用者定案，2026-10-17）。改成 1 即開放。越鬥越長不受這個開關影響。 */
#define SPECIALS_OPEN 0

/* 特殊招式是否可用：開關打開，或是設了 fonxan/npc_specials 的 NPC（例如柳東蘆，
 * 使用者定案，2026-10-28）。 */
private int specials_open(object me)
{
    return SPECIALS_OPEN || (!userp(me) && me->query("fonxan/npc_specials"));
}

private void create()
{
    seteuid(getuid());

    art_id    = "fonxansword";
    art_name  = "封山劍法";
    art_usage = "sword";
    art_desc  = "封山派最受推崇的劍法，變化多端、攻守兼備，"
                + "不以一擊之威見長，而以綿長的後勁取勝。";
    art_bonus = 0;

    moves = ({
        ([ "name": "峰迴路轉", "attack": 7, "defense": 5, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N使一招「峰迴路轉」﹐手中$w如一條銀蛇般刺向$n的$l" ]),
        ([ "name": "空山鳥語", "attack": 7, "defense": 4, "force": 2, "bonus": 0, "damage_type": "割傷",
           "action": "$N使出封山劍法中的「空山鳥語」﹐劍光霍霍斬向$n的$l" ]),
        ([ "name": "御風而行", "attack": 6, "defense": 7, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N一招「御風而行」﹐身形隨劍滑出數尺﹐手中$w斬向$n的$l" ]),
        ([ "name": "旭日東昇", "attack": 9, "defense": 3, "force": 2, "bonus": 5, "damage_type": "刺傷",
           "action": "$N手中$w中宮直進﹐一式「旭日東昇」對準$n的$l刺出一劍" ]),
        ([ "name": "金光瀉地", "attack": 9, "defense": 2, "force": 2, "bonus": 5, "damage_type": "刺傷",
           "action": "$N縱身一躍﹐手中$w一招「金光瀉地」對準$n的$l斜斜刺出一劍" ]),
        ([ "name": "童子引路", "attack": 5, "defense": 4, "force": 3, "bonus": 10, "damage_type": "刺傷",
           "action": "$N的$w憑空一指﹐一招「童子引路」刺向$n的$l" ]),
        ([ "name": "柳暗花明", "attack": 7, "defense": 5, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N手中$w向外一分﹐使一招「柳暗花明」反手對準$n$l一劍刺去" ]),
        ([ "name": "空谷秋虹", "attack": 6, "defense": 6, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N橫劍上前﹐身形一轉手中$w使一招「空谷秋虹」畫出一道光弧斬向$n的$l" ]),
    });

    art_entry     = 20;
    art_entry_msg = "你終於練成了封山劍法。\n";

    ma_coef  = 10;
    mm_start = 30;
    mm_minus = 29;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("fonxansword");
    setup();
}

/* 可當劍術、雙手劍使用，不能當招架（使用者定案，2026-10-17）。 */
int valid_enable(string usage) { return usage == "sword" || usage == "twohanded sword"; }

/* 學習條件：封山派弟子。 */
int valid_learn(object me)
{
    if( !is_faction(me, "fighter.fonxan") )
        return notify_fail("你不是封山派弟子﹐無法修練封山劍法。\n");
    return 1;
}

/* 對手還在、同一個地方、自己還清醒，才繼續施展後續的招式。 */
int can_follow(object me, object opponent)
{
    return objectp(me) && objectp(opponent) && living(me)
        && environment(me) == environment(opponent);
}

string weapon_name(object weapon)
{
    return objectp(weapon) ? weapon->name() : "劍";
}

/* 八荒：封山劍法 100 級起，出劍前 20% × 等級/200；連出三招普通劍招（各自可閃躲、格擋），總耗精 150%。 */
int do_bahuang(object me, object opponent, object weapon)
{
    int i, damage;

    message_vision(HIW "$N手中" + weapon_name(weapon) + "寒光一閃﹐『" HIY " 八 荒 " HIW "』劍式已然出手！\n" NOR,
        me, opponent, 1);
    for( i = 0; i < 3; i++ ) {
        if( !can_follow(me, opponent) ) break;
        damage = special_hit(me, opponent, weapon, pick_move(me), 100, 50, 0);
    }
    return damage;
}

/* 封字訣：封山劍法 120 級起，出劍前 15% × 等級/200；一道劍氣（可閃躲、格擋，傷害 100%），
 * 打中時對方停頓 1 回合。 */
int do_fengzi(object me, object opponent, object weapon)
{
    int damage;

    message_vision(HIW "\n$N一聲清嘯﹐正是封山絕學『" HIY "封" HIW "』字訣﹐如虹劍氣自劍尖而出激得狂風驟起將$n逼退數步。\n\n" NOR,
        me, opponent, 1);
    damage = special_hit(me, opponent, weapon,
        ([ "name": "封字訣", "attack": 9, "defense": 3, "force": 2, "bonus": 0, "damage_type": "割傷",
           "action": "$N的劍氣直撲$n的$l" ]), 100, 100, 0);
    if( damage > 0 && objectp(opponent) ) opponent->start_busy(1);
    return damage;
}

void attack_using(object me, object opponent, object weapon)
{
    int lv, damage;

    if( !opponent ) return;
    if( !objectp(weapon) ) weapon = 0;

    if( specials_open(me) ) {
        lv = me->query_skill("fonxansword", 1);
        if( lv >= 100 && random(100) < scaled_chance(me, 20) ) {
            damage = do_bahuang(me, opponent, weapon);
            after_strike(me, opponent, weapon, 0, damage);
            return;
        }
        if( lv >= 120 && random(100) < scaled_chance(me, 15) ) {
            damage = do_fengzi(me, opponent, weapon);
            after_strike(me, opponent, weapon, 0, damage);
            return;
        }
    }
    ::attack_using(me, opponent, weapon);
}

/* 出劍之後的特效。 */
void after_strike(object me, object opponent, object weapon, mapping m, int damage)
{
    int lv, n, ratio;

    if( !can_follow(me, opponent) ) return;
    lv = me->query_skill("fonxansword", 1);

    /* 越鬥越長：enable 封山派內功時，出劍後 10% × 等級/200，回復精 封山劍法/2（開放中）。 */
    if( me->skill_mapped("force") == "fonxan force" && random(100) < scaled_chance(me, 10) ) {
        message_vision(HIW "$N的封山劍法配合封山派的正宗心法﹐雖於激烈的惡鬥之中﹐竟是越鬥精力越長！\n" NOR, me, 0, 1);
        me->supplement_stat("gin", lv / 2);
    }

    if( !specials_open(me) ) return;

    /* 無形劍氣：封山劍法 150 級起、有 enable 內功；對方停頓中時必定發動。
     * 不能閃躲、格擋，防具照扣；傷害 = 內功等級 × 3/2（舊版劍芒）。 */
    if( lv >= 150 && me->skill_mapped("force") && opponent->is_busy() ) {
        message_vision(HIW "$N手中" + weapon_name(weapon) + "迸發出一道淩厲無儔的無形劍氣﹐勢如射日長虹向$n奔騰而去！\n" NOR,
            me, opponent, 1);
        n = me->query_skill("force") * 3 / 2;
        n -= opponent->resist_damage(n, me);
        if( n > 0 ) opponent->receive_damage(n, me, me);
        show_status(opponent, kee_ratio(opponent));
        if( !can_follow(me, opponent) ) return;
    }

    /* 上弦月 → 下弦月：封山劍法 180 級起，出劍後 10% × 等級/200。
     * 上弦月傷害 200%、不能閃躲格擋、對方停頓 1 回合；下弦月扣對方精 封山劍法等級；
     * 之後自己停頓 1 回合。發動時不再搶攻。 */
    if( lv >= 180 && random(100) < scaled_chance(me, 10) ) {
        message_vision(HIW "\n$N一躍而起﹐盡全身餘力揮出封山絕學『" HIY "上 弦 月" HIW "』﹐綿綿劍氣劃破天際席捲$n週身要害！\n\n" NOR,
            me, opponent, 1);
        special_hit(me, opponent, weapon, 0, 200, 100, HIT_MUST | HIT_SILENT);
        ratio = kee_ratio(opponent);
        show_status(opponent, ratio);
        if( !can_follow(me, opponent) ) return;
        opponent->start_busy(1);
        message_vision(HIW "\n借劍氣未散之力﹐$N迴轉手中" + weapon_name(weapon) + "半空之中順勢使出『" HIY "下 弦 月" HIW "』﹐道道劍氣衝入$n週身脈絡！\n\n" NOR,
            me, opponent, 1);
        opponent->consume_stat("gin", lv, me);
        message_vision(HIW "\n劍氣過後﹐$N手中" + weapon_name(weapon) + "劍身入地半尺支撐$N﹐緩解施展劍氣對自身經脈的衝擊。\n\n" NOR, me, 0, 1);
        me->start_busy(1);
        return;
    }

    /* 搶攻：封山劍法 60 級起，出劍後 15% × 等級/200；多出一招普通劍招。 */
    if( lv >= 60 && random(100) < scaled_chance(me, 15) ) {
        message_vision(HIW "$N劍氣已過劍招尚未使老﹐身形飄然上前發起搶攻！\n" NOR, me, 0, 1);
        COMBAT_D->fight(me, opponent, art_id, make_action(pick_move(me), 100), weapon);
    }
}
