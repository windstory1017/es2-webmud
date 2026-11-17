/*---
description: 黑山八卦掌（NEW 新版武功寫法）。哭笑門掌法，招式沉穩、爆發力強，傷害略高於風嘯拳法；無特功。
             設計見 docs/martial_arts/哭笑門_大邪心法與其他武功.md。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

private void create()
{
    seteuid(getuid());

    art_id    = "hesanzhang";
    art_name  = "黑山八卦掌";
    art_usage = "unarmed";
    art_desc  = "依八卦方位遊走的掌法，腳步沉穩、掌力渾厚，出手不多卻一擊便是爆發。";
    art_bonus = 0;

    /* 八招對應八卦：乾（黑虎偷心）、坤、震、巽、坎、離、艮、兌。 */
    moves = ({
        ([ "name": "黑虎偷心", "attack": 7, "defense": 3, "force": 3, "bonus": 10, "damage_type": "瘀傷",
           "action": "$N足下一轉搶入乾位﹐左掌虛晃一記﹐右掌五指微曲自肋下穿出﹐「黑虎偷心」直取$n的$l" ]),
        ([ "name": "坤地載物", "attack": 5, "defense": 6, "force": 3, "bonus": 5, "damage_type": "瘀傷",
           "action": "$N退守坤位﹐雙臂圈出一道大弧﹐一招「坤地載物」兩掌似托千斤沉沉推向$n的$l" ]),
        ([ "name": "震雷貫耳", "attack": 6, "defense": 3, "force": 3, "bonus": 10, "damage_type": "瘀傷",
           "action": "$N雙掌一合一分﹐正是「震雷貫耳」﹐身形已閃到震位﹐兩道掌緣自側面夾擊$n的$l" ]),
        ([ "name": "巽風掃葉", "attack": 7, "defense": 4, "force": 2, "bonus": 5, "damage_type": "瘀傷",
           "action": "$N隨著巽位的步子旋身而進﹐左右雙掌輪番拍出﹐「巽風掃葉」一掌快似一掌印向$n的$l" ]),
        ([ "name": "坎水沉山", "attack": 6, "defense": 5, "force": 2, "bonus": 5, "damage_type": "瘀傷",
           "action": "$N沉肩墜肘﹐腳下悄然滑入坎位﹐一招「坎水沉山」雙掌平平推向$n的$l" ]),
        ([ "name": "離火燎原", "attack": 7, "defense": 3, "force": 3, "bonus": 5, "damage_type": "瘀傷",
           "action": "$N右掌掄圓﹐帶著一股熱浪循離位方向劈出﹐「離火燎原」呼的一聲撲向$n的$l" ]),
        ([ "name": "艮山壓頂", "attack": 5, "defense": 4, "force": 3, "bonus": 10, "damage_type": "瘀傷",
           "action": "$N矮身擰腰﹐穩穩立成艮位樁勢﹐一掌自下托起再猛然翻落﹐「艮山壓頂」重重按向$n的$l" ]),
        ([ "name": "兌澤迴波", "attack": 6, "defense": 4, "force": 2, "bonus": 5, "damage_type": "瘀傷",
           "action": "$N側身讓過正面﹐繞向兌位﹐掌影層層疊疊如水波盪開﹐「兌澤迴波」連環拍向$n的$l" ]),
    });

    art_entry     = 20;
    art_entry_msg = "你終於練成了黑山八卦掌。\n";

    ma_coef  = 10;
    mm_start = 31;
    mm_minus = 30;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("hesanzhang");
    setup();
}

/* 學習條件：哭笑門弟子。 */
int valid_learn(object me)
{
    if( !is_faction(me, "fighter.huge") )
        return notify_fail("你不是哭笑門弟子﹐黑山八卦掌的路數你學不來。\n");
    return 1;
}
