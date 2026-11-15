/*---
description: 五虎斷門刀（NEW 新版武功寫法）。孟家祖傳的單手刀法，孟家兄弟（孟驚鴻、孟昭展）專用，玩家不能學；
             5 招加絕招『五 虎 斷 門』：NPC 每次出招有 10% 機率觸發，必中、不能閃躲格擋，對手 delay 3 tick。
             設計見 docs/martial_arts/孟家_五虎斷門刀.md。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

#define SKILL_ID    "five-tiger blade"
#define TICK_SECS   2          // 1 tick = 2 秒
#define CD_TICKS    6          // 絕招冷卻（使用者確認，2026-11-14）
#define TRIGGER_PCT 10         // NPC 每次出招觸發絕招的機率（%，使用者指示，2026-11-14）
#define DELAY_TICKS 3          // 對手 delay 3 tick（使用者指示）

private void create()
{
    seteuid(getuid());

    art_id    = SKILL_ID;
    art_name  = "五虎斷門刀";
    art_usage = "blade";
    art_desc  = "孟家祖傳的刀法，由開國將軍孟翔溪所創，刀勢兇悍霸道，招招只攻不守；"
                + "催到極處更能使出絕學『五 虎 斷 門』，封死對手的退路。";
    art_bonus = 0;

    moves = ({
        ([ "name": "惡虎剜心", "attack": 8, "defense": 1, "force": 1, "bonus": 12, "damage_type": "割傷",
           "action": "$N矮身欺近﹐一招「惡虎剜心」﹐$w自肋下反撩而上狠狠剜進$n的$l" ]),
        ([ "name": "伏虎斷腰", "attack": 7, "defense": 0, "force": 1, "bonus": 15, "damage_type": "砍傷",
           "action": "$N腰身猛擰﹐$w橫掃千鈞﹐一招「伏虎斷腰」勢如開山直要將$n的$l一刀兩斷" ]),
        ([ "name": "猛虎撲崖", "attack": 6, "defense": 0, "force": 1, "bonus": 15, "damage_type": "砍傷",
           "action": "$N縱身躍起﹐雙手擎$w過頂﹐「猛虎撲崖」挾著千斤力道﹐當頭朝$n的$l狠命劈落" ]),
        ([ "name": "飛虎穿林", "attack": 10, "defense": 2, "force": 1, "bonus": 10, "damage_type": "刺傷",
           "action": "$N足尖一點貼地竄出﹐一招「飛虎穿林」﹐$w人刀合一直捅進$n的$l半尺" ]),
        ([ "name": "裂爪撕風", "attack": 8, "defense": 3, "force": 1, "bonus": 12, "damage_type": "割傷",
           "action": "$N左手五指箕張如虎爪﹐右手$w隨後劈落﹐「裂爪撕風」刀爪齊下﹐要把$n的$l撕得皮開肉綻" ]),
    });

    // 孟家兄弟專用，沒有學成門檻與升級獎勵。
    art_entry     = 1;
    art_entry_msg = "你終於練成了五虎斷門刀﹐刀勢兇悍霸道﹐隱隱有五道虎影隨刀而生。\n";

    DAEMON_D->register_skill_daemon(SKILL_ID);
    setup();
}

/* 單手刀（使用者定案，2026-11-14）。 */
int valid_enable(string usage)
{
    return usage == "blade";
}

/* 玩家不能學（使用者定案，2026-11-14）；NPC 在自己的檔案裡直接 set_skill。 */
int valid_learn(object me)
{
    return notify_fail("這是孟家的祖傳刀法﹐外人學不得。\n");
}

/* 絕招的敘述：整段亮黃色、一行不斷行，前後各空一行；失敗（氣力不繼）一樣用黃色。 */
private void say_yellow(string msg, object me, object victim)
{
    message_vision("\n" + HIY + msg + NOR + "\n\n", me, victim);
}

/* 『五 虎 斷 門』：必中、不能閃躲格擋，成功時對手 delay 3 tick；消耗氣最大值的 1/10，自身停頓 1 tick，冷卻 6 tick。 */
int door_break(object me, object victim)
{
    int cost;

    if( me->query_temp("five_tiger/cd") > time() ) return 0;
    cost = me->query_stat_maximum("kee") / 10;
    me->set_temp("five_tiger/cd", time() + CD_TICKS * TICK_SECS);
    if( me->query_stat("kee") <= cost ) {
        say_yellow("$N一聲暴喝﹐想使出孟家祖傳絕學『五 虎 斷 門』﹐卻氣力不繼﹐刀勢一滯！", me, victim);
        return 1;
    }
    me->consume_stat("kee", cost);
    say_yellow("$N一聲暴喝﹐使出孟家祖傳絕學『五 虎 斷 門』刀風過處退路盡斷﹐$n被封得動彈不得！", me, victim);
    me->start_busy(1);
    victim->start_busy(DELAY_TICKS);
    return 1;
}

/* NPC 每次出招有 10% 機率先使出絕招，再照常出一招。 */
void attack_using(object me, object opponent, object weapon)
{
    if( !opponent ) return;
    if( !userp(me) && random(100) < TRIGGER_PCT ) door_break(me, opponent);
    ::attack_using(me, opponent, weapon);
}
