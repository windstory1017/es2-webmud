/*---
description: 柳家拳法（NEW 新版武功寫法）。封山派空手武功，柳東蘆的家傳拳法。
             以楊柳隨風擺盪、柔韌不折、垂絲拂水、柳絮紛飛的姿態出招，強度一般，沒有特技。
             設計見 docs/martial_arts/封山派_封山劍法與倒亂七星步法.md。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

private void create()
{
    seteuid(getuid());

    art_id    = "liuh-ken";
    art_name  = "柳家拳法";
    art_usage = "unarmed";
    art_desc  = "柳東蘆的家傳拳法，取楊柳隨風擺盪、柔韌不折的姿態化為五式，"
                + "以柔克剛，是封山派弟子打熬根基的拳腳功夫。";
    art_bonus = 0;

    moves = ({
        ([ "name": "隨風擺柳", "attack": 6, "defense": 4, "force": 1, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N腰身一擰﹐雙臂如柳枝迎風擺盪﹐一招「隨風擺柳」﹐右掌順勢甩向$n的$l" ]),
        ([ "name": "柳絲纏腕", "attack": 7, "defense": 3, "force": 1, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N使出「柳絲纏腕」﹐左手柔若無骨地繞過來勢﹐右手五指趁隙扣向$n的$l" ]),
        ([ "name": "垂柳拂水", "attack": 5, "defense": 5, "force": 1, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N身形低伏﹐雙掌貼著地面平平掠過﹐正是「垂柳拂水」﹐輕飄飄地掃向$n的$l" ]),
        ([ "attack": 5, "defense": 3, "force": 2, "bonus": 5, "damage_type": "瘀傷",
           "action": "$N上身後仰如柳枝被大風壓彎﹐待風勢一盡猛然彈回﹐右拳直搗$n的$l" ]),
        ([ "attack": 6, "defense": 4, "force": 1, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N雙掌虛虛實實﹐掌影如漫天柳絮紛飛﹐其中一掌悄然拍向$n的$l" ]),
    });

    /* 學成：累積 10000 點，gain 時直接練成 25 級；學成那一級也給獎勵（比照七巧如意手）。 */
    art_entry           = 25;
    art_entry_threshold = 10000;
    art_entry_reward    = 1;
    art_entry_msg       = "你練成了柳家拳法。\n";

    /* 武術造詣 等級 × 10；41 級起武學之道 (等級 − 40) × 10。 */
    ma_coef  = 10;
    mm_start = 41;
    mm_minus = 40;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("liuh-ken");
    setup();
}

/* 學習條件：封山派弟子。 */
int valid_learn(object me)
{
    if( !is_faction(me, "fighter.fonxan") )
        return notify_fail("你不是封山派弟子﹐無法修練柳家拳法。\n");
    return 1;
}
