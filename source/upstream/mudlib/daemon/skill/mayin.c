/*---
description: 梅影身法。冷梅莊閃躲身法，在雪上踏步、揣摩梅花身形而來，重飄逸輕身；
             閃避機率照 ES2 原本公式。設計見 docs/martial_arts/冷梅莊_劍士武功.md。
---*/
#include <ansi.h>
inherit SKILL;

/* 閃避敘述：接在對方出招敘述後面，只有閃過時才出現。
 * $N 是出招的人，$n 是閃避的人。 */
string *dodge_actions = ({
    "$n足尖在雪地上輕輕一點﹐身形如一片梅瓣隨風飄起﹐$N這一擊從$n腳下掠了過去",
    "$n腳下踏著細碎的步子﹐雪地上只留下幾點淺淺的梅花印﹐人已飄到$N身側",
    "$n身子微微一側﹐如梅枝在風中輕擺﹐$N的攻勢擦著衣角落了空",
    "$n輕飄飄地向後滑出數尺﹐衣袂翻飛如落梅迴雪﹐讓$N撲了個空",
    "$n身形一晃﹐宛如梅影搖曳﹐$N眼前一花﹐這一擊已然落空",
    "$n踏雪無痕﹐身形繞著$N輕盈一轉﹐如梅花在枝頭迴旋﹐飄然避開",
});

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("mayin");
    setup();
}

int valid_enable(string usage)
{
    return usage == "dodge";
}

/* 學習條件：冷梅莊弟子。 */
int valid_learn(object me)
{
    if( me->query("custom_faction") != "fighter.lunmay" )
        return notify_fail("你不是冷梅莊弟子﹐無法修練梅影身法。\n");
    return 1;
}

// 學成：累積 500 點時 gain，直接練成 10 級。
int query_entry_level() { return 10; }
int query_entry_threshold() { return 500; }

void skill_completed(object me, string sk)
{
    tell_object(me, HIY "你的梅影身法已經初有小成。\n" NOR);
}

/* 每級獎勵（學成那一級起）：武術造詣 等級 × 10；
 * 41 級起武學之道 (等級 - 40) × 10。 */
void skill_advanced(object me, string sk)
{
    int lv;

    if( !userp(me) ) return;
    lv = me->query_skill("mayin", 1);
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
