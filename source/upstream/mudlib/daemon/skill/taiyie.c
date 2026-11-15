/*---
description: 太乙劍法（NEW 新版武功寫法）。道士共通劍法，可當劍術與招架；五招，沒有特技。
             設計見 docs/martial_arts/道士共通_太乙劍法.md。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

private void create()
{
    seteuid(getuid());

    art_id    = "taiyie";
    art_name  = "太乙劍法";
    art_usage = "sword";
    art_desc  = "太乙觀流傳的劍法，劍意取太乙一氣化為兩儀、三才、四象、五行之變，"
                + "劍勢清靜自然，攻守兼備，是道士都能修練的劍術。";
    art_bonus = 0;

    moves = ({
        ([ "name": "太乙含光", "attack": 7, "defense": 4, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N劍交左手﹐右手劍訣一領﹐一招「太乙含光」﹐$w隱隱泛起一層清光﹐緩緩遞向$n的$l" ]),
        ([ "name": "兩儀迴環", "attack": 7, "defense": 4, "force": 1, "bonus": 3, "damage_type": "割傷",
           "action": "$N腳踏陰陽兩位﹐手中$w畫出一黑一白兩個圓圈﹐正是「兩儀迴環」﹐圈轉處倏地削向$n的$l" ]),
        ([ "name": "三才鼎立", "attack": 8, "defense": 3, "force": 2, "bonus": 5, "damage_type": "刺傷",
           "action": "$N上步沉肩﹐$w分點天、地、人三路﹐「三才鼎立」三劍連環﹐最後一劍直取$n的$l" ]),
        ([ "name": "四象生變", "attack": 7, "defense": 4, "force": 1, "bonus": 3, "damage_type": "割傷",
           "action": "$N身形一轉﹐劍勢隨四象流轉﹐忽而迎面﹐忽而繞背﹐一式「四象生變」﹐斜撩$n的$l" ]),
        ([ "name": "五行歸元", "attack": 8, "defense": 3, "force": 2, "bonus": 5, "damage_type": "刺傷",
           "action": "$N袖袍一拂﹐$w挾五行之勢盤旋而下﹐「五行歸元」氣象森嚴﹐收束處劍尖點向$n的$l" ]),
    });

    /* 學成：累積到 20 級門檻時 gain，直接練成 20 級；學成那一級起每級給獎勵。 */
    art_entry        = 20;
    art_entry_reward = 1;
    art_entry_msg    = "你將太乙劍法的五招融會貫通﹐劍意中隱隱透出一股清靜自然之氣。\n";

    /* 每級給武術造詣 等級 × 10；41 級起武學之道 (等級 - 40) × 10。 */
    ma_coef  = 10;
    mm_start = 41;
    mm_minus = 40;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("taiyie");
    setup();
}

int valid_enable(string usage) { return usage == "sword" || usage == "parry"; }

/* 學習條件：道士（含茅山派），不限門派。 */
int valid_learn(object me)
{
    if( me->query("class") != "taoist" )
        return notify_fail("太乙劍法是道士的劍法﹐你無法修練。\n");
    return 1;
}

/* 招架：拆招卸力敘述（$n 是招架者，$N 是攻擊者）；招名與出招相同，敘述不同。 */
string parry_message(object me, object attacker, int full)
{
    object w;
    string wn;

    w = me->query_temp("weapon/sword");
    wn = objectp(w) ? w->name() : "雙手";
    return ({
        "$n劍尖微微一垂﹐一招「太乙含光」清光流轉﹐將$N的攻勢輕輕引開",
        "$n手中" + wn + "畫圓相迎﹐「兩儀迴環」一黑一白﹐把$N的來勢圈在劍影之中",
        "$n腳下釘穩﹐" + wn + "上下左右連封三處﹐「三才鼎立」令$N的攻勢無從下手",
        "$n身隨劍轉﹐一式「四象生變」前後呼應﹐將$N這一招層層化解",
        "$n袖袍一拂﹐「五行歸元」勁力盡數收回劍上﹐$N的招式像打在棉絮裡",
    })[random(5)];
}
