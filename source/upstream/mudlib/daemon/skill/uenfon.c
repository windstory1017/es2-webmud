/*---
description: 雲峰筆招（NEW 新版武功寫法）。共通匕法，可用在匕法與左手匕法；六招以書法意境入招，
             100 級起出招前有機會快速點穴，讓對手停頓 2 回合。設計見 docs/martial_arts/共通_雲峰筆招.md。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

private void create()
{
    seteuid(getuid());

    art_id    = "uenfon";
    art_name  = "雲峰筆招";
    art_usage = "dagger";
    art_desc  = "一套以書法意境入招的短兵武功，時而端正沉穩、時而連綿不斷、時而縱橫恣肆，"
                + "出手之間如揮毫落紙，更能趁隙點人穴道。";
    art_bonus = 0;

    moves = ({
        ([ "attack": 6, "defense": 4, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N一招「峰巒初展」﹐$w端端正正一橫一豎﹐收勢時直點$n的$l" ]),
        ([ "attack": 7, "defense": 3, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N手腕輕轉﹐使出「雲出無心」﹐$w如行雲流水般牽連不斷地劃向$n的$l" ]),
        ([ "attack": 7, "defense": 2, "force": 1, "bonus": 5, "damage_type": "刺傷",
           "action": "$N醉步一晃﹐$w縱橫恣肆如龍蛇亂走﹐這一記「千峰亂雲」戳向$n的$l" ]),
        ([ "attack": 6, "defense": 3, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N手中$w先是一頓﹐隨即平拖而出﹐「雁尾橫雲」收勢一挑正挑向$n的$l" ]),
        ([ "attack": 6, "defense": 3, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N手腕圓轉不斷﹐「盤峰迴雲」$w兜了個圈子﹐倏地刺向$n的$l" ]),
        ([ "attack": 5, "defense": 2, "force": 2, "bonus": 10, "damage_type": "刺傷",
           "action": "$N縱身躍起凌空一點﹐使出「雲峰墜石」﹐$w挾著全身之力砸落$n的$l" ]),
    });

    /* 學成：累積到 10 級門檻時 gain，直接練成 10 級；學成那一級起每級給武術造詣 等級 × 10。 */
    art_entry        = 10;
    art_entry_reward = 1;
    art_entry_msg    = "你終於練成了雲峰筆招﹐出手起落之間﹐隱隱已有雲山疊嶂的氣象。\n";

    /* 每級武術造詣 等級 × 10；41 級起武學之道 (等級 − 40) × 10。 */
    ma_coef  = 10;
    mm_start = 41;
    mm_minus = 40;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("uenfon");
    setup();
}

/* 可用在匕法與左手匕法。 */
int valid_enable(string usage) { return usage == "dagger" || usage == "secondhand dagger"; }

/* 學習條件：讀書識字 90；不限種族、職業、性別。 */
int valid_learn(object me)
{
    if( me->query_skill("literate", 1) < 90 )
        return notify_fail("你讀書識字的根基尚淺﹐還參不透雲峰筆招的奧妙。\n");
    return 1;
}

/* 點穴的機率：悟性 × 20 ÷ 35 %，最低 5%、最高 20%。 */
int point_chance(object me)
{
    int c;

    c = me->query_attr("int") * 20 / 35;
    if( c < 5 ) c = 5;
    if( c > 20 ) c = 20;
    return c;
}

/* 點穴：雲峰筆招 100 級起，出招前判定，對手停頓 2 回合，沒有傷害。
 * 每回合只判定一次（該回合第一次用雲峰筆招出手時），單握、左握、雙持機率一致。 */
void attack_using(object me, object opponent, object weapon)
{
    int t;

    if( opponent && objectp(weapon) && me->query_skill("uenfon", 1) >= 100 ) {
        t = time();
        if( me->query_temp("uenfon/round") != t ) {
            me->set_temp("uenfon/round", t);
            if( random(100) < point_chance(me) ) {
                message_vision("\n" HIY "$N手中" + weapon->name() + "疾顫﹐"
                    + "眨眼間已連點$n身上三處大穴﹐$n手足一麻登時動彈不得﹗\n\n" NOR,
                    me, opponent);
                opponent->start_busy(2);
            }
        }
    }
    ::attack_using(me, opponent, weapon);
}
