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

    /* 八招對應八卦：乾天門、坤地、震雷、巽風、坎（黑虎偷心：坎為北方屬黑、主隱伏，取暗中偷襲）、離火、艮山、兌澤。 */
    moves = ({
        ([ "name": "天門洞開", "attack": 6, "defense": 4, "force": 3, "bonus": 10, "damage_type": "瘀傷",
           "action": "$N搶入乾位﹐雙臂大開大闔﹐一招「天門洞開」掌勢剛猛如天崩﹐當頭劈向$n的$l" ]),
        ([ "name": "雙掌托鼎", "attack": 5, "defense": 6, "force": 3, "bonus": 5, "damage_type": "瘀傷",
           "action": "$N退守坤位﹐腰身一沉﹐雙臂自下圈起﹐一招「雙掌托鼎」兩掌如托千斤重鼎般推向$n的$l" ]),
        ([ "name": "雙風貫耳", "attack": 6, "defense": 3, "force": 3, "bonus": 10, "damage_type": "瘀傷",
           "action": "$N身形已閃到震位﹐雙掌自兩側同時掄起﹐一招「雙風貫耳」掌緣挾著呼嘯夾擊$n的$l" ]),
        ([ "name": "長風掃葉", "attack": 7, "defense": 4, "force": 2, "bonus": 5, "damage_type": "瘀傷",
           "action": "$N隨著巽位的步子旋身而進﹐左右雙掌輪番拍出﹐「長風掃葉」一掌快似一掌印向$n的$l" ]),
        ([ "name": "黑虎偷心", "attack": 7, "defense": 3, "force": 3, "bonus": 10, "damage_type": "瘀傷",
           "action": "$N悄然滑入坎位﹐身形伏低如暗夜潛行﹐左掌虛晃﹐一招「黑虎偷心」右掌自肋下穿出直取$n的$l" ]),
        ([ "name": "星火燎原", "attack": 7, "defense": 3, "force": 3, "bonus": 5, "damage_type": "瘀傷",
           "action": "$N右掌掄圓﹐帶著一股熱浪循離位方向劈出﹐「星火燎原」呼的一聲撲向$n的$l" ]),
        ([ "name": "泰山壓頂", "attack": 5, "defense": 4, "force": 3, "bonus": 10, "damage_type": "瘀傷",
           "action": "$N矮身擰腰﹐穩穩立成艮位樁勢﹐一掌自下托起再猛然翻落﹐「泰山壓頂」重重按向$n的$l" ]),
        ([ "name": "層浪拍岸", "attack": 6, "defense": 4, "force": 2, "bonus": 5, "damage_type": "瘀傷",
           "action": "$N側身讓過正面﹐繞向兌位﹐掌影層層疊疊如水波盪開﹐「層浪拍岸」一浪接一浪拍向$n的$l" ]),
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
