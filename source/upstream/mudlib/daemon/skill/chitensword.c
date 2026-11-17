/*---
description: 殘蟻五劍訣（NEW 新版武功寫法）。哭笑門劍法，招式狠辣、專攻要害，傷害略遜冷梅劍法與封山劍法；無特功。
             設計見 docs/martial_arts/哭笑門_大邪心法與其他武功.md。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

private void create()
{
    seteuid(getuid());

    art_id    = "chitensword";
    art_name  = "殘蟻五劍訣";
    art_usage = "sword";
    art_desc  = "青邪宮殘蟻師太傳下的劍法，五式劍招狠辣刁鑽，專挑要害下手。";
    art_bonus = 0;

    moves = ({
        ([ "name": "毒蛇吐信", "attack": 7, "defense": 2, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N身形微沉﹐手中$w貼著地面斜挑而上﹐一招「毒蛇吐信」劍尖悄沒聲息地刺向$n的$l" ]),
        ([ "name": "殘月斷魂", "attack": 8, "defense": 1, "force": 1, "bonus": 3, "damage_type": "割傷",
           "action": "$N左掌虛按﹐右手$w反腕一抹﹐正是「殘月斷魂」﹐劍光如一彎冷月削向$n的$l" ]),
        ([ "name": "寒星連珠", "attack": 7, "defense": 2, "force": 2, "bonus": 0, "damage_type": "刺傷",
           "action": "$N手腕連抖﹐$w顫出點點寒星﹐「寒星連珠」一點緊似一點啄向$n的$l" ]),
        ([ "name": "迴風絞鋒", "attack": 6, "defense": 3, "force": 1, "bonus": 3, "damage_type": "割傷",
           "action": "$N踏步欺身﹐$w繞出一個圈子將$n的招式一絞﹐一招「迴風絞鋒」劍尖順勢遞進$n的$l" ]),
        ([ "name": "斷腸一線", "attack": 8, "defense": 1, "force": 2, "bonus": 5, "damage_type": "刺傷",
           "action": "$N縱身躍起﹐人劍合為一線自上而下直落﹐「斷腸一線」$w挾著凌厲殺氣穿向$n的$l" ]),
    });

    art_entry     = 20;
    art_entry_msg = "你終於練成了殘蟻五劍訣。\n";

    /* 每級武術造詣 等級 × 10；31 級起武學之道 (等級 - 30) × 10。 */
    ma_coef  = 10;
    mm_start = 31;
    mm_minus = 30;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("chitensword");
    setup();
}

/* 學習條件：哭笑門弟子。 */
int valid_learn(object me)
{
    if( !is_faction(me, "fighter.huge") )
        return notify_fail("你不是哭笑門弟子﹐殘蟻五劍訣的路數你學不來。\n");
    return 1;
}
