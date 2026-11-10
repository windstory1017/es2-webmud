/*---
description: 倒亂七星步法。封山派閃躲步法，七式對應北斗七星；閃避機率照 ES2 原本公式。
             100 級起被攻擊時有機會發動「七星殘影」，扣對手的精並回復自己。
             閃避敘述取自「梅影風 vs 柳東蘆」對戰紀錄。
             設計見 docs/martial_arts/封山派_封山劍法與倒亂七星步法.md。
---*/
#include <ansi.h>
inherit SKILL;

/* 閃避敘述：接在對方出招敘述後面，只有閃過時才出現。
 * $N 是出招的人，$n 是閃避的人。 */
string *dodge_actions = ({
    "$n側身退開﹐使出「天樞無相」倒亂步法令$N撲了個空",
    "$n斜斜地踏出一步﹐一招「天璇步罡」讓$N撲了個空",
    "$n身影一晃﹐正是一招「天璣流光」﹐輕輕巧巧地閃了開去",
    "$n一招「天權中隱」﹐側身退開﹐輕易地避過了這招",
    "$n朝$N疾竄而出﹐正是一招「玉衡待問」搶在頭裡躲開了$N的攻擊",
    "$n一招「開陽非虛」﹐身形飄忽往後騰挪了半尺﹐避開了這招",
    "$n身不動腿不抬﹐一招「搖光逝影」原地轉了一圈﹐讓$N的攻勢硬生生地落了個空",
});

void seven_shadows(object me, object attacker);

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("chaos-steps");
    setup();
}

int valid_enable(string usage)
{
    return usage == "dodge";
}

/* 學習條件：封山派弟子。 */
int valid_learn(object me)
{
    if( me->query("custom_faction") != "fighter.fonxan" )
        return notify_fail("你不是封山派弟子﹐無法修練倒亂七星步法。\n");
    return 1;
}

// 學成：照一般門檻累積到 10 級時 gain，直接練成 10 級。
int query_entry_level() { return 10; }

void skill_completed(object me, string sk)
{
    tell_object(me, HIY "你已經掌握了倒亂七星步法。\n" NOR);
}

/* 每級獎勵（超過學成等級才給）：武術造詣 等級 × 10；
 * 41 級起武學之道 (等級 - 40) × 10。 */
void skill_advanced(object me, string sk)
{
    int lv;

    if( !userp(me) ) return;
    lv = me->query_skill("chaos-steps", 1);
    if( lv <= 10 ) return;
    me->gain_score("martial art", lv * 10);
    if( lv > 40 ) me->gain_score("martial mastery", (lv - 40) * 10);
}

/*
 * 閃避機率照 ES2 原本的公式，步法只提供有效閃避值。
 * 這裡先選好一句閃避敘述交給出招的一方；戰鬥程式只有在真的閃過時
 * 才會把這句接到出招敘述後面。
 */
int dodge_using(object me, int ability, int strength, object from)
{
    object attacker;

    if( objectp(me) && objectp(from) ) {
        attacker = from;
        if( !from->is_character()
        &&  objectp(environment(from)) && environment(from)->is_character() )
            attacker = environment(from);

        if( attacker->is_character() && attacker != me ) {
            attacker->set_temp("defend_message",
                dodge_actions[random(sizeof(dodge_actions))]);
            seven_shadows(me, attacker);
        }
    }

    return me->query_skill("dodge");
}

/* 七星殘影：倒亂七星步法 100 級起，被攻擊時 30% 發動（不影響這一招能否閃過）。
 * 扣對手精 = (random(自己的機敏) + 1) + 倒亂七星步法等級 / 5，
 * 自己回復扣掉的 1/3（使用者定案，2026-10-17）。 */
void seven_shadows(object me, object attacker)
{
    int sk, dex, n;

    sk = me->query_skill("chaos-steps", 1);
    if( sk < 100 || random(100) >= 30 ) return;

    dex = me->query_attr("dex");
    n = (dex > 0 ? random(dex) : 0) + 1 + sk / 5;

    message_vision(CYN "\n$N清嘯一聲﹐提氣疾轉﹐竟化為七道殘影在$n面前按七星方位狂旋起來！！\n\n" NOR,
        me, attacker);
    attacker->consume_stat("gin", n, me);
    me->supplement_stat("gin", n / 3);
    message_vision("$N被七道殘影繞得頭暈眼花﹐精神一陣恍惚。\n", attacker);
}
