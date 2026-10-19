/*---
description: 火雲奇門步法。朱衣派閃躲步法，八式對應奇門八門，身法如火雲飄捲；閃避機率照 ES2 原本公式，沒有特技。
             設計見 docs/martial_arts/道士_清河映星劍與奇門步法.md。
---*/
#include <ansi.h>
inherit SKILL;

/* 閃避敘述：接在對方出招敘述後面，只有閃過時才出現。
 * $N 是出招的人，$n 是閃避的人。 */
string *dodge_actions = ({
    "$n踏入「休門」﹐身形如一縷輕煙向後飄開﹐$N這一擊落了個空",
    "$n一步轉入「生門」﹐火雲般的衣袂一捲﹐已閃到$N身側",
    "$n斜踏「傷門」﹐身子低伏﹐讓$N的攻勢從頭頂掠過",
    "$n腳走「杜門」﹐倏地橫移數尺﹐$N連衣角都沒碰著",
    "$n步入「景門」﹐身形一晃化作數道殘影﹐$N不知該打哪一個",
    "$n反踏「死門」﹐看似自投險地﹐卻在間不容髮之際側身避開",
    "$n輕叱一聲踏上「驚門」﹐身形陡然拔起﹐從$N頭頂翻了過去",
    "$n由「開門」飄然而出﹐如火雲隨風捲散﹐輕輕巧巧地避過了這招",
});

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("eight_steps");
    setup();
}

int valid_enable(string usage)
{
    return usage == "dodge";
}

/* 學習條件：朱衣派弟子。 */
int valid_learn(object me)
{
    if( me->query("custom_faction") != "taoist.fire" )
        return notify_fail("你不是朱衣派弟子﹐無法修練火雲奇門步法。\n");
    return 1;
}

// 學成：照一般門檻累積到 10 級時 gain，直接練成 10 級。
int query_entry_level() { return 10; }

void skill_completed(object me, string sk)
{
    tell_object(me, HIY "你踏遍奇門八門﹐火雲奇門步法已然小成。\n" NOR);
}

/* 每級獎勵（學成那一級起）：武術造詣 等級 × 10；41 級起武學之道 (等級 − 40) × 10。 */
void skill_advanced(object me, string sk)
{
    int lv;

    if( !userp(me) ) return;
    lv = me->query_skill("eight_steps", 1);
    if( lv < 10 ) return;
    me->gain_score("martial art", lv * 10);
    if( lv > 40 ) me->gain_score("martial mastery", (lv - 40) * 10);
}

/*
 * 閃避機率照 ES2 原本的公式，身法只提供有效閃避值。
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

        if( attacker->is_character() && attacker != me )
            attacker->set_temp("defend_message",
                dodge_actions[random(sizeof(dodge_actions))]);
    }

    return me->query_skill("dodge");
}
