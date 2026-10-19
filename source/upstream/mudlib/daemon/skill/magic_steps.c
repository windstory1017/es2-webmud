/*---
description: 陰陽奇門步。紫衣派閃躲步法，依陰陽、奇門方位移形換位；閃避機率照 ES2 原本公式，沒有特技。
             設計見 docs/martial_arts/道士_清河映星劍與奇門步法.md。
---*/
#include <ansi.h>
inherit SKILL;

/* 閃避敘述：接在對方出招敘述後面，只有閃過時才出現。
 * $N 是出招的人，$n 是閃避的人。 */
string *dodge_actions = ({
    "$n足踏陰位﹐身形一轉已移向陽位﹐$N這一擊落在空處",
    "$n依奇門方位斜退半步﹐似虛似實﹐讓$N撲了個空",
    "$n身影一分為二﹐一陰一陽各向兩側滑開﹐$N竟不知該向哪一邊出手",
    "$n腳下暗合八卦﹐繞著$N輕輕一轉﹐悄然避開了這招",
});

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("magic steps");
    setup();
}

int valid_enable(string usage)
{
    return usage == "dodge";
}

/* 學習條件：紫衣派弟子。 */
int valid_learn(object me)
{
    if( me->query("custom_faction") != "taoist.thunder" )
        return notify_fail("你不是紫衣派弟子﹐無法修練陰陽奇門步。\n");
    return 1;
}

// 學成：照一般門檻累積到 10 級時 gain，直接練成 10 級。
int query_entry_level() { return 10; }

void skill_completed(object me, string sk)
{
    tell_object(me, HIY "你參透了陰陽奇門步的方位變化﹐身形已能隨陰陽流轉。\n" NOR);
}

/* 每級獎勵（學成那一級起）：武術造詣 等級 × 10；41 級起武學之道 (等級 − 40) × 10。 */
void skill_advanced(object me, string sk)
{
    int lv;

    if( !userp(me) ) return;
    lv = me->query_skill("magic steps", 1);
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
