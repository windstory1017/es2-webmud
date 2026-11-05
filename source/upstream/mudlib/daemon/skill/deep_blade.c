/*---
description: 深空刀法（NEW 新版武功寫法）。雪吟莊刀法，招式為「X招＋數字」的數字刀；
             60 級起出刀後有機會使出乘法口訣的無視防禦絕招。設計見 docs/martial_arts/雪吟莊_武功.md。
---*/
#include <ansi.h>

inherit "/std/martial_art";

#define FACTION         "fighter.snow"
#define SPECIAL_LEVEL   60      // 絕招從幾級起出現
#define SPECIAL_CHANCE  66      // 絕招觸發機率（%）

private void create()
{
    seteuid(getuid());

    art_id    = "deep blade";
    art_name  = "深空刀法";
    art_usage = "blade";
    art_desc  = "雪吟莊的刀法，招名都是一個「招」字配上一個數目，刀路看似雜亂卻暗藏算計；"
                + "練到深處，刀招忽變，歪歪斜斜的一刀竟教人避無可避。";
    art_bonus = 0;

    moves = ({
        ([ "name": "快招二十五", "attack": 9, "defense": 3, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N身子一轉﹐$w舞出一團金光﹐「" HIY "快招二十五" NOR "」狂風驟雨般攻向$n的$l" ]),
        ([ "name": "狠招三十六", "attack": 7, "defense": 2, "force": 2, "bonus": 10, "damage_type": "割傷",
           "action": "$N左掌在刀背上一拍﹐「" HIR "狠招三十六" NOR "」挾著勁風直劈$n的$l" ]),
        ([ "name": "怪招四十九", "attack": 8, "defense": 4, "force": 1, "bonus": 5, "damage_type": "割傷",
           "action": "$N腳步一歪似要跌倒﹐「" HIM "怪招四十九" NOR "」卻從腋下反撩$n的$l" ]),
        ([ "name": "險招六十四", "attack": 8, "defense": 1, "force": 2, "bonus": 10, "damage_type": "割傷",
           "action": "$N不閃不避欺身直進﹐一招「" HIC "險招六十四" NOR "」橫斬$n的$l" ]),
        ([ "name": "奇招八十一", "attack": 9, "defense": 3, "force": 1, "bonus": 5, "damage_type": "割傷",
           "action": "$N的$w左右手一換﹐「" HIG "奇招八十一" NOR "」刀光一分為二斫向$n的$l" ]),
    });

    art_entry_msg = "你練成了深空刀法﹐刀招之中隱隱已有一番算計。\n";

    ma_coef  = 10;
    mm_start = 30;
    mm_minus = 29;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("deep blade");
    setup();
}

/* 學習條件：雪吟莊弟子（拜師就能學）。 */
int valid_learn(object me)
{
    if( !is_faction(me, FACTION) )
        return notify_fail("你不是雪吟莊弟子﹐無法修練深空刀法。\n");
    return 1;
}

/* 乘法口訣：例如 2×2 → 二二得四、5×8 → 五八四十、9×9 → 九九八十一。 */
string rhyme(int a, int b)
{
    int n = a * b;

    if( n < 10 ) return chinese_number(a) + chinese_number(b) + "得" + chinese_number(n);
    if( n == 10 ) return chinese_number(a) + chinese_number(b) + "一十";
    return chinese_number(a) + chinese_number(b) + chinese_number(n);
}

/* 絕招：深空刀法 60 級起，出刀造成傷害後 66% 觸發。
 * 兩個數字各從二到九（小的在前），追加一刀 這一刀傷害 × (100 + 乘積)% 的傷害，
 * 不能閃躲、格擋，也不再扣防具（以已經扣過防具的這一刀為準）。 */
void after_strike(object me, object opponent, object weapon, mapping m, int damage)
{
    int a, b, n;

    if( damage <= 0 || !objectp(opponent) ) return;
    if( me->query_skill("deep blade", 1) < SPECIAL_LEVEL ) return;
    if( !living(opponent) || environment(opponent) != environment(me) ) return;
    if( random(100) >= SPECIAL_CHANCE ) return;

    a = 2 + random(8);
    b = 2 + random(8);
    if( a > b ) { n = a; a = b; b = n; }
    n = damage * (100 + a * b) / 100;

    message_vision("$N刀招忽變﹐使出深空刀法絕招「" HIW + rhyme(a, b) + NOR "」﹐"
        + "忽然歪歪斜斜地砍出一刀﹐竟逼得$n避無可避！\n", me, opponent);
    opponent->receive_damage(n, me, me);
    if( objectp(opponent) ) show_status(opponent, kee_ratio(opponent));
}
