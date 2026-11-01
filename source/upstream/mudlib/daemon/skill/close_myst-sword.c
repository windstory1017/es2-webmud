/*---
description: 小步玄劍（NEW 新版武功寫法）。步玄派劍法，可 enable 在劍術、左手劍術與拆招卸力之法。
             飄逸、快劍、君子之風，帶竹的意境但不寫「竹」字；暫無特殊能力
             （招架回精氣留給書生進階）。
             設計見 docs/martial_arts/步玄派_門派設計.md。
---*/
#include <ansi.h>

inherit "/std/martial_art";

private void create()
{
    seteuid(getuid());

    art_id    = "close myst-sword";
    art_name  = "小步玄劍";
    art_usage = "sword";
    art_desc  = "步玄派的入門劍法，出劍輕快飄逸，守多於攻，"
                + "劍意清朗勁直，自有一股君子之風。";
    art_bonus = 0;

    moves = ({
        ([ "name": "虛懷納風", "attack": 6, "defense": 6, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N身形微側﹐一招「虛懷納風」讓過來勢﹐$w順手刺向$n的$l" ]),
        ([ "name": "清風拂林", "attack": 7, "defense": 4, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N青衫一飄﹐使出「清風拂林」$w輕削而過﹐劍光掠向$n的$l" ]),
        ([ "name": "瀟瀟夜雨", "attack": 7, "defense": 3, "force": 1, "bonus": 5, "damage_type": "刺傷",
           "action": "$N手腕連抖﹐劍尖點點如夜雨打葉﹐正是「瀟瀟夜雨」罩向$n的$l" ]),
        ([ "name": "寧折不彎", "attack": 7, "defense": 3, "force": 2, "bonus": 5, "damage_type": "刺傷",
           "action": "$N$w中宮直進﹐一式「寧折不彎」勁直無回﹐逕取$n的$l" ]),
        ([ "name": "青雲直上", "attack": 6, "defense": 5, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N拔地而起﹐一招「青雲直上」﹐$w由下而上撩向$n的$l" ]),
        ([ "name": "風搖不折", "attack": 5, "defense": 7, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N上身隨風一晃﹐「風搖不折」順勢迴身﹐$w抹過$n的$l" ]),
        ([ "name": "清影搖窗", "attack": 7, "defense": 6, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N身形飄忽如窗前清影﹐「清影搖窗」$w一晃而至﹐輕輕點向$n的$l" ]),
    });

    art_entry     = 20;
    art_entry_msg = "你終於練成了小步玄劍﹐出劍輕靈飄逸﹐自有一股清風拂林的君子氣度。\n";

    // 升級獎勵（使用者定案，2026-11-01）：學成之後每級武術造詣 等級 × 10；
    // 武學之道 (等級 − 40) × 10，41 級起。
    ma_coef  = 10;
    mm_start = 41;
    mm_minus = 40;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("close myst-sword");
    setup();
}

/* 劍術、左手劍術、拆招卸力之法（使用者定案，2026-11-01）。 */
int valid_enable(string usage)
{
    return usage == "sword" || usage == "secondhand sword" || usage == "parry";
}

/* 學習條件：步玄派弟子。 */
int valid_learn(object me)
{
    if( !is_faction(me, "scholar.myst") )
        return notify_fail("你不是步玄派弟子﹐無法修練小步玄劍。\n");
    return 1;
}

/* 招架：拆招卸力敘述（$n 是招架者，$N 是攻擊者）。手上沒有劍時用系統的格擋敘述。 */
string parry_message(object me, object attacker, int full)
{
    object w;
    string wn;

    w = me->query_temp("weapon/sword");
    if( !objectp(w) ) w = me->query_temp("weapon/secondhand sword");
    if( !objectp(w) ) return 0;
    wn = w->name();
    return ({
        "$n手中" + wn + "斜斜一引﹐將$N的來勢帶偏了半尺",
        "$n以一招「風搖不折」順著來勢一讓﹐$N這一招便落在空處",
        "$n劍身貼著來勢輕輕一粘﹐腕底迴轉將$N的攻勢卸到一旁",
        "$n退步收劍﹐劍尖點在$N腕上﹐這一招登時使老了",
    })[random(4)];
}
