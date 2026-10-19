/*---
description: 清河映星劍（NEW 新版武功寫法）。紫衣派劍法，劍意取清河映星與雷、電、雨之象，沒有特技。
             設計見 docs/martial_arts/道士_清河映星劍與奇門步法.md。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

private void create()
{
    seteuid(getuid());

    art_id    = "reflecting star sword";
    art_name  = "清河映星劍";
    art_usage = "sword";
    art_desc  = "紫衣派的劍法，劍光如星落清河，出手時兼有驚雷疾電之勢、"
                + "細雨連綿之密，是紫衣派弟子防身的劍術。";
    art_bonus = 0;

    moves = ({
        ([ "name": "星落清河", "attack": 7, "defense": 4, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N劍尖斜斜一沉﹐一招「星落清河」﹐劍光點點如星墜河心﹐刺向$n的$l" ]),
        ([ "name": "驚雷乍響", "attack": 8, "defense": 3, "force": 2, "bonus": 5, "damage_type": "刺傷",
           "action": "$N足下一頓﹐手中$w嗡然作響﹐「驚雷乍響」挾著一聲裂空銳嘯﹐直點$n的$l" ]),
        ([ "name": "疾電穿雲", "attack": 9, "defense": 2, "force": 1, "bonus": 5, "damage_type": "割傷",
           "action": "$N身形一晃﹐手中$w化作一道白電﹐正是「疾電穿雲」﹐由下而上斜挑$n的$l" ]),
        ([ "name": "細雨連珠", "attack": 7, "defense": 5, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N手腕連抖﹐劍尖顫出點點寒芒﹐一招「細雨連珠」如綿綿細雨灑落﹐籠住$n的$l" ]),
        ([ "name": "河漢倒懸", "attack": 6, "defense": 5, "force": 2, "bonus": 5, "damage_type": "割傷",
           "action": "$N迴身倒轉$w﹐劍光如銀河倒瀉、驟雨傾盆﹐一式「河漢倒懸」由上而下削向$n的$l" ]),
    });

    /* 學成：照一般門檻累積到 20 級時 gain，直接練成 20 級；學成那一級起每級給獎勵。 */
    art_entry        = 20;
    art_entry_reward = 1;
    art_entry_msg    = "你終於練成了清河映星劍﹐劍光流轉之間﹐隱隱已有星河雷雨之勢。\n";

    /* 每級給武術造詣 等級 × 10；41 級起武學之道 (等級 − 40) × 10。 */
    ma_coef  = 10;
    mm_start = 41;
    mm_minus = 40;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("reflecting star sword");
    setup();
}

/* 學習條件：紫衣派弟子。 */
int valid_learn(object me)
{
    if( !is_faction(me, "taoist.thunder") )
        return notify_fail("你不是紫衣派弟子﹐無法修練清河映星劍。\n");
    return 1;
}
