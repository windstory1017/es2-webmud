/*---
description: 傲梅暗劍訣（NEW 新版武功寫法）。冷梅莊左手劍法，可 enable 在副手劍或副手匕首。
             設計見 docs/martial_arts/冷梅莊_劍士武功.md。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

private void create()
{
    seteuid(getuid());

    art_id    = "advance_lunmay";
    art_name  = "傲梅暗劍訣";
    art_usage = "secondhand sword";
    art_desc  = "冷梅莊的左手劍法，快劍連綿；出招時有機會施展殺招【梅二方劍】、【梅四方劍】，"
                + "殺招累積到七成火候，便能發出「愛霜恨雪」。";
    art_bonus = 0;

    moves = ({
        ([ "name": "梅影幢幢", "attack": 9, "defense": 2, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N右手劍招倏地一收﹐左手一劍「梅影幢幢」閃電般劃向$n$l" ]),
        ([ "name": "傲梅藏鋒", "attack": 8, "defense": 3, "force": 1, "bonus": 5, "damage_type": "刺傷",
           "action": "$N趁著冷梅劍法攻勢未盡﹐左手冷不防一招「傲梅藏鋒」刺向$n$l" ]),
        ([ "name": "梅厲風行", "attack": 10, "defense": 1, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N左劍暴然向前疾探﹐一招「梅厲風行」又快又準地刺向$n$l" ]),
        ([ "name": "暗劍難防", "attack": 9, "defense": 2, "force": 2, "bonus": 5, "damage_type": "刺傷",
           "action": "$N右手劍招一停﹐左手劍招「暗劍難防」跟著遞出﹐斜刺$n$l" ]),
        ([ "name": "梅落何方", "attack": 8, "defense": 3, "force": 2, "bonus": 5, "damage_type": "刺傷",
           "action": "$N右手一劍刺出﹐身子順勢一個迴翻﹐左手一招「梅落何方」﹐$w對準$n$l一陣疾刺" ]),
        ([ "name": "雪蛇弄梅", "attack": 10, "defense": 2, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N一聲清嘯﹐左手$w突然彈跳而出﹐「雪蛇弄梅」一招瞬間貼身遊至$n$l" ]),
    });

    art_entry     = 20;
    art_entry_msg = "你練成了傲梅暗劍訣。\n";
    level_msgs    = ([
        80:  "你施展傲梅暗劍訣中的殺招【梅二方劍】！\n",
        100: "你施展傲梅暗劍訣中的殺招【梅四方劍】！\n",
    ]);

    ma_coef  = 10;
    mm_start = 30;
    mm_minus = 29;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("advance_lunmay");
    setup();
}

int valid_enable(string usage)
{
    return usage == "secondhand sword" || usage == "secondhand dagger";
}

/* 學習條件：等級 30、寒梅心法 100 級。 */
int valid_learn(object me)
{
    if( me->query_level() < 30 )
        return notify_fail("你的火候未到﹐還參不透傲梅暗劍訣的精要。\n");
    if( me->query_skill("hainmay force", 1) < 100 )
        return notify_fail("你的寒梅心法尚未大成﹐無法修練傲梅暗劍訣。\n");
    return 1;
}

int in_mirror(object me)
{
    return me->query_temp("hainmay/mirror") && me->skill_mapped("force") == "hainmay force";
}

/* 連續兩下以上、只顯示體力狀態的殺招。 */
private void silent_hits(object me, object victim, object weapon, int n, int pct,
    int gin_pct, int flags)
{
    int *ratios = ({}), i, r;

    for( i = 0; i < n; i++ ) {
        if( !objectp(victim) ) break;
        special_hit(me, victim, weapon, 0, pct, i ? 0 : gin_pct, flags | HIT_SILENT);
        ratios += ({ kee_ratio(victim) });
    }
    foreach(r in ratios) show_status(victim, r);
}

/* 冰凍：明鏡止水狀態下，機率 50% × 傲梅暗劍訣/200。對手停頓 1 回合，
 * 並受普攻 30% 的傷害（不能閃躲、格擋，防具照樣減傷），耗精 110%。 */
private void freeze(object me, object victim, object weapon)
{
    message_vision("$N招式之間寒冷的傲梅劍氣將$n凍得面起白霜﹐渾身發抖 ...\n", me, victim);
    victim->start_busy(1);
    silent_hits(me, victim, weapon, 1, 30, 110, HIT_MUST);
}

/* 梅二方劍：80 級起，機率 30% × 等級/200。兩下各為普攻 115%，不能閃躲、格擋，
 * 防具照樣減傷；耗精 120%；累積 2 點。 */
private void two_way(object me, object victim, object weapon)
{
    message_vision(HIW "$N施展傲梅暗劍訣中的殺招【梅二方劍】！" NOR "\n"
        "$N全力施展快劍﹐幻化出了二道劍氣將$n團團圍住！\n", me, victim);
    me->add_temp("advance_lunmay/points", 2);
    silent_hits(me, victim, weapon, 2, 115, 120, HIT_MUST);
}

/* 梅四方劍：100 級起，機率 60% × 等級/200。瞬間連出四招（一般攻擊，可被閃躲、格擋，
 * 防具照樣減傷）；一招被閃躲或格擋，剩下的就不再施展。耗精 140%；發動即累積 4 點。 */
private void four_way(object me, object victim, object weapon)
{
    int i, result;

    message_vision(HIW "$N施展傲梅暗劍訣中的殺招【梅四方劍】！" NOR "\n"
        "$N全力施展快劍﹐幻化出了四道劍氣將$n團團圍住！\n", me, victim);
    me->add_temp("advance_lunmay/points", 4);
    for( i = 0; i < 4; i++ ) {
        if( !objectp(victim) || environment(victim) != environment(me) ) break;
        result = special_hit(me, victim, weapon, 0, 100, i ? 0 : 140, 0);
        if( result < 0 || (objectp(victim) && victim->query_temp("parry_result")) ) break;
    }
}

/* 愛霜恨雪：殺招點數累積到 7 點時發動並清空。普攻 300%，不能閃躲、格擋，
 * 無視防具；耗精 150%。 */
private void love_frost(object me, object victim, object weapon)
{
    me->delete_temp("advance_lunmay/points");
    message_vision("$N意猶未盡﹐手上" + (objectp(weapon) ? weapon->name() : "劍")
        + "突然斜裡翻出劍尖急顫！\n"
        HIW "$N施展傲梅暗劍訣中的殺招「愛霜恨雪」！一股無形的冷冽氣勁朝著$n直射而出！" NOR "\n",
        me, victim, 1);   // 整句一行、不自動斷行（使用者指示，2026-10-31）
    silent_hits(me, victim, weapon, 1, 300, 150, HIT_MUST | HIT_NO_ARMOR);
}

void after_strike(object me, object opponent, object weapon, mapping m, int damage)
{
    int sk;

    if( !objectp(opponent) || environment(opponent) != environment(me) ) return;
    sk = me->query_skill("advance_lunmay", 1);

    if( in_mirror(me) && random(100) < scaled_chance(me, 50) ) {
        freeze(me, opponent, weapon);
        if( !objectp(opponent) ) return;
    }

    if( sk >= 100 && random(100) < scaled_chance(me, 60) )
        four_way(me, opponent, weapon);
    else if( sk >= 80 && random(100) < scaled_chance(me, 30) )
        two_way(me, opponent, weapon);
    else return;

    if( objectp(opponent) && me->query_temp("advance_lunmay/points") >= 7 )
        love_frost(me, opponent, weapon);
}
