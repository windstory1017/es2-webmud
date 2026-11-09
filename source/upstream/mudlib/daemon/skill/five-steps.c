/*---
description: 五行迷蹤步。素衣派閃躲步法，五式依金木水火土排列；沒有特攻，閃避機率照 ES2 原本公式。
             41 級起每級增加武學之道。
             設計見 docs/martial_arts/素衣派_五行迷蹤步.md。
---*/
#include <ansi.h>
inherit SKILL;

/* 閃避敘述：接在對方出招敘述後面，只有閃過時才出現。
 * $N 是出招的人，$n 是閃避的人。五式閃避率相當，隨機選一句；只有敘述，沒有特攻。 */
string *dodge_actions = ({
    "$n足踏金位﹐一招「金風過隙」身形薄如利刃﹐自$N的招式縫隙中穿了過去",
    "$n身子如古木迎風一晃﹐「枝動影移」已閃到$N的側旁﹐讓這一招落了個空",
    "$n腳下斜斜一滑﹐正是「流水無形」﹐$N的攻勢像打在水上﹐半點著不得力",
    "$n輕叱一聲﹐「烈焰騰挪」身形倏地竄起數尺﹐$N這一招擦著衣角燒了個空",
    "$n雙膝微沉﹐使出「厚土承勢」﹐隨$N的來勢穩穩一側身﹐勁力盡數卸在空處",
});

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("five-steps");
    setup();
}

int valid_enable(string usage)
{
    return usage == "dodge";
}

/* 學習條件：素衣派弟子，拜師後找上清道長學習（使用者定案，2026-11-09）。 */
int valid_learn(object me)
{
    if( me->query("custom_faction") != "taoist.freeze" )
        return notify_fail("你不是素衣派弟子﹐無法修練五行迷蹤步。\n");
    return 1;
}

// 學成：照一般門檻累積到 10 級時 gain，直接練成 10 級。
int query_entry_level() { return 10; }

void skill_completed(object me, string sk)
{
    tell_object(me, HIY "你已經掌握了五行迷蹤步。\n" NOR);
}

/* 每級獎勵（超過學成等級才給）：武術造詣 等級 × 10；
 * 41 級起武學之道 (等級 - 40) × 10。 */
void skill_advanced(object me, string sk)
{
    int lv;

    if( !userp(me) ) return;
    lv = me->query_skill("five-steps", 1);
    if( lv <= 10 ) return;
    me->gain_score("martial art", lv * 10);
    if( lv > 40 ) me->gain_score("martial mastery", (lv - 40) * 10);
}

/* 閃避機率照 ES2 原本的公式，步法只提供有效閃躲值；這裡先選好一句閃避敘述。 */
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
