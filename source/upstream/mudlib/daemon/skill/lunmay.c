/*---
description: 冷梅劍法（NEW 新版武功寫法）。冷梅莊右手劍法，也可 enable 在招架上。
             設計見 docs/martial_arts/冷梅莊_劍士武功.md。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

private void create()
{
    seteuid(getuid());

    art_id    = "lunmay";
    art_name  = "冷梅劍法";
    art_usage = "sword";
    art_desc  = "冷梅莊的看家劍法，攻守兼備；招架得手後能順勢反擊，"
                + "配合寒梅心法的明鏡止水，更能使出精義【寒霜三千里】。";
    art_bonus = 0;

    moves = ({
        ([ "name": "力能貫日", "attack": 9, "defense": 3, "force": 2, "bonus": 5, "damage_type": "刺傷",
           "action": "$N使出「力能貫日」﹐手中$w劃出一個耀眼的光輪﹐$N身形回斂凝力於劍﹐透過光輪直刺$n的$l" ]),
        ([ "name": "寒風飄雪", "attack": 7, "defense": 5, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N使出「寒風飄雪」﹐手中$w隨著身形由左往右順勢一帶﹐幻出一道劍浪劃向$n的$l" ]),
        ([ "name": "花枝亂顫", "attack": 8, "defense": 4, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N使出一招「花枝亂顫」﹐$w搖晃著化為數道劍影從各個不定的方位分襲向$n的$l" ]),
        ([ "name": "白虹經天", "attack": 6, "defense": 3, "force": 3, "bonus": 10, "damage_type": "割傷",
           "action": "$N順勢轉身一招「白虹經天」﹐手中$w劍身化成一道白光挾著銳嘯以劇力萬鈞之勢直劈向$n的$l" ]),
        ([ "name": "漫天花影", "attack": 8, "defense": 5, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N一式「漫天花影」使將出來﹐手中的$w倏地朝空一立﹐劍尖由上而下顫出了無數的劍花罩向了$n的$l" ]),
        ([ "name": "柳暗花明", "attack": 7, "defense": 6, "force": 2, "bonus": 5, "damage_type": "割傷",
           "action": "$N身形一矮﹐往前大跨一步﹐手中$w一招「柳暗花明」倏地劍勢一頓﹐由虛轉實由下而上回撩$n的$l" ]),
    });

    art_entry     = 20;
    art_entry_msg = "你終於練成了冷梅劍法。\n";
    level_msgs    = ([ 100: "你領悟了冷梅劍法精義【寒霜三千里】！\n" ]);

    ma_coef  = 10;
    mm_start = 30;
    mm_minus = 29;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("lunmay");
    setup();
}

int valid_enable(string usage) { return usage == "sword" || usage == "parry"; }

/* 學習條件：冷梅莊弟子。 */
int valid_learn(object me)
{
    if( !is_faction(me, "fighter.lunmay") )
        return notify_fail("你不是冷梅莊弟子﹐無法修練冷梅劍法。\n");
    return 1;
}

/* 明鏡止水狀態（寒梅心法 exert mirror，且內功 enable 寒梅心法）。 */
int in_mirror(object me)
{
    return me->query_temp("hainmay/mirror") && me->skill_mapped("force") == "hainmay force";
}

/* 寒霜三千里：冷梅劍法 100 級起，明鏡止水狀態下，右手出招前判定；
 * 機率 30% × 冷梅劍法/200。對手停頓 2 回合，沒有傷害。 */
void attack_using(object me, object opponent, object weapon)
{
    if( opponent && me->query_skill("lunmay", 1) >= 100 && in_mirror(me)
    &&  random(100) < scaled_chance(me, 30) ) {
        message_vision(HIW "\n$N催運寒梅心法使出【寒霜三千里】劍氣﹐"
            + (objectp(weapon) ? weapon->name() : "劍") + "劍刃嗤嗤做響﹐"
            + "$n全身瞬間蒙上一層白霜！" NOR "\n\n", me, opponent, 1);   // 整句一行、不自動斷行（2026-10-31）
        opponent->start_busy(2);
    }
    ::attack_using(me, opponent, weapon);
}

/* 招架：拆招卸力敘述（$n 是招架者，$N 是攻擊者）。 */
string parry_message(object me, object attacker, int full)
{
    object w;
    string wn;

    w = me->query_temp("weapon/sword");
    wn = objectp(w) ? w->name() : "雙手";
    return ({
        "$n應以一招「寒梅蕭蕭」﹐擋住了$N的攻勢",
        "$n手中" + wn + "急轉﹐一招「橫梅四處」將$N的來勢一一封住",
        "$n手中" + wn + "一揮﹐一式「寒霜隔梅」將$N的攻勢化解掉",
        "$n手裡遞出一招「冷梅紛紛」將$N的所有攻勢化解掉",
    })[random(4)];
}

/* 招架反擊：格擋成功（含完全擋下）後，機率 50% × 冷梅劍法/200，右手持劍才會發動。
 * 反擊兩下，不能閃躲、格擋，防具照樣減傷；耗精合計等於一次普攻。 */
void counter_attack(object me, object attacker, object weapon)
{
    int *ratios = ({}), i;

    if( !objectp(me) || !objectp(attacker) || !living(me) ) return;
    if( environment(me) != environment(attacker) ) return;
    if( me->query_temp("weapon/sword") != weapon ) return;

    message_vision("$N架開$n的攻擊﹐順勢舞出二道寒冷的劍氣直指$n週身要害！\n", me, attacker);
    for( i = 0; i < 2; i++ ) {
        if( !objectp(attacker) ) break;
        special_hit(me, attacker, weapon, 0, 100, i ? 0 : 100, HIT_MUST | HIT_SILENT);
        ratios += ({ kee_ratio(attacker) });
    }
    foreach(i in ratios) show_status(attacker, i);
}

void parry_success(object me, object attacker, int full)
{
    object weapon;

    if( !objectp(attacker) ) return;
    if( !objectp(weapon = me->query_temp("weapon/sword")) ) return;
    if( random(100) >= scaled_chance(me, 50) ) return;
    // 等對方這一擊的訊息顯示完再反擊。
    call_out("counter_attack", 0, me, attacker, weapon);
}
