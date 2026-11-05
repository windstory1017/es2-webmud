/*---
description: 六陰追魂劍法（NEW 新版武功寫法）。雪吟莊劍法，劍路陰森飄忽，也可 enable 在招架上。
             設計見 docs/martial_arts/雪吟莊_武功.md。
---*/
#include <ansi.h>

inherit "/std/martial_art";

#define FACTION "fighter.snow"

private void create()
{
    seteuid(getuid());

    art_id    = "six-chaos-sword";
    art_name  = "六陰追魂劍法";
    art_usage = "sword";
    art_desc  = "雪吟莊的劍法，身法飄忽、劍走陰柔，招招如鬼魅纏身；"
                + "也能用來拆招卸力，將對手的攻勢引到一旁。";
    art_bonus = 0;

    moves = ({
        ([ "name": "厲鬼纏身", "attack": 8, "defense": 4, "force": 1, "bonus": 5, "damage_type": "割傷",
           "action": "$N身形一轉﹐一招「厲鬼纏身」﹐$w斬向$n的後心" ]),
        ([ "name": "陰風拂面", "attack": 7, "defense": 5, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N左袖一拂捲起陣陣陰風﹐「陰風拂面」劍尖點向$n的$l" ]),
        ([ "name": "孤魂索命", "attack": 9, "defense": 3, "force": 1, "bonus": 5, "damage_type": "刺傷",
           "action": "$N腳下飄忽無聲﹐使出「孤魂索命」﹐$w直刺$n的$l" ]),
        ([ "name": "鬼影穿林", "attack": 8, "defense": 5, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N身形忽左忽右﹐「鬼影穿林」劍光從斜裏削向$n的$l" ]),
        ([ "name": "黃泉引路", "attack": 7, "defense": 6, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N劍尖下垂似要收招﹐「黃泉引路」卻自下撩向$n的$l" ]),
        ([ "name": "六陰鎖魂", "attack": 8, "defense": 4, "force": 2, "bonus": 10, "damage_type": "割傷",
           "action": "$N手腕連抖﹐劍光圈成六道陰氣﹐「六陰鎖魂」絞向$n的$l" ]),
    });

    art_entry_msg = "你練成了六陰追魂劍法﹐劍光過處﹐隱隱帶著一股森森陰氣。\n";

    ma_coef  = 10;
    mm_start = 30;
    mm_minus = 29;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("six-chaos-sword");
    setup();
}

int valid_enable(string usage) { return usage == "sword" || usage == "parry"; }

/* 學習條件：雪吟莊弟子（拜師就能學）。 */
int valid_learn(object me)
{
    if( !is_faction(me, FACTION) )
        return notify_fail("你不是雪吟莊弟子﹐無法修練六陰追魂劍法。\n");
    return 1;
}

/* 招架：拆招卸力敘述（$n 是招架者，$N 是攻擊者）。 */
string parry_message(object me, object attacker, int full)
{
    return ({
        "$n劍身斜斜一引﹐貼著$N的來勢一帶﹐將這一招卸到身側",
        "$n手腕一沉﹐劍尖畫了個半圈﹐輕輕巧巧地黏開了$N的攻勢",
        "$n身形向後一飄﹐劍鋒順勢橫封﹐恰好擋在來路之上",
    })[random(3)];
}
