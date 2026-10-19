/*---
description: 七巧如意手（NEW 新版武功寫法）。冷梅莊空手武功，以梅樹、梅枝、梅花的形態
             與冷冽意境出招，沒有特技。設計見 docs/martial_arts/冷梅莊_劍士武功.md。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

private void create()
{
    seteuid(getuid());

    art_id    = "seven";
    art_name  = "七巧如意手";
    art_usage = "unarmed";
    art_desc  = "冷梅莊的入門拳掌功夫，取梅樹、梅枝與梅花的形態化為七式掌指，"
                + "出手清冷俐落，是打熬根基的基礎武功。";
    art_bonus = 0;

    moves = ({
        ([ "attack": 6, "defense": 3, "force": 1, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N左掌護胸﹐右掌五指微張如梅枝橫斜﹐掌緣帶著一縷寒意削向$n的$l" ]),
        ([ "attack": 5, "defense": 4, "force": 2, "bonus": 5, "damage_type": "瘀傷",
           "action": "$N身形一沉﹐雙足穩如老梅盤根﹐右拳自下而上﹐筆直撞向$n的$l" ]),
        ([ "attack": 7, "defense": 3, "force": 0, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N雙手十指交錯一分﹐似梅花五瓣次第綻開﹐輕輕拂過$n的$l" ]),
        ([ "attack": 6, "defense": 4, "force": 1, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N手腕一抖﹐指尖如雪壓梅枝後倏然彈起﹐疾點$n的$l" ]),
        ([ "attack": 4, "defense": 6, "force": 1, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N左掌虛引﹐右掌隨後悄然按出﹐掌風清冷如雪後梅香﹐直印$n的$l" ]),
        ([ "attack": 5, "defense": 3, "force": 2, "bonus": 5, "damage_type": "瘀傷",
           "action": "$N五指併攏如含苞梅蕊﹐一縮一放之間﹐指勁已透入$n的$l" ]),
        ([ "attack": 6, "defense": 5, "force": 1, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N雙臂舒展如寒梅迎風﹐兩掌一上一下交替拍出﹐點點寒意罩住$n的$l" ]),
    });

    /* 學成：累積 10000 點，gain 時直接練成 25 級；學成那一級也給獎勵。 */
    art_entry           = 25;
    art_entry_threshold = 10000;
    art_entry_reward    = 1;
    art_entry_msg       = "你練成了七巧如意手。\n";

    ma_coef  = 10;
    mm_start = 41;
    mm_minus = 40;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("seven");
    setup();
}

/* 學習條件：冷梅莊弟子。 */
int valid_learn(object me)
{
    if( !is_faction(me, "fighter.lunmay") )
        return notify_fail("你不是冷梅莊弟子﹐無法修練七巧如意手。\n");
    return 1;
}
