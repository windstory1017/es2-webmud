/*---
description: 迅雷斧法（NEW 新版武功寫法）。共通斧法，可用在單手斧與左手斧；六招交替，
             90 級起雙手持斧時依行動力發出連擊。設計見 docs/martial_arts/共通_迅雷斧法.md。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

private void create()
{
    seteuid(getuid());

    art_id    = "soonlight axe";
    art_name  = "迅雷斧法";
    art_usage = "axe";
    art_desc  = "一套以快打快的斧法，斧勢如迅雷疾電，身形起落間連劈帶砍，"
                + "雙手各持一斧時更能一斧緊接一斧，叫人應接不暇。";
    art_bonus = 0;

    moves = ({
        ([ "attack": 6, "defense": 3, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N腳下一錯身形已到$n跟前﹐「迅雷驚蟄」斧刃挾著風聲由上而下疾劈$n的$l" ]),
        ([ "attack": 7, "defense": 2, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N迴手一揚﹐使出「電光裂石」﹐斧光才一閃已劈到$n的$l" ]),
        ([ "attack": 6, "defense": 4, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N斜跨一步搶到$n身側﹐一招「風行雷動」反手一斧橫掃$n的$l" ]),
        ([ "attack": 7, "defense": 2, "force": 1, "bonus": 5, "damage_type": "割傷",
           "action": "$N手腕急翻連砍三斧﹐正是「霹靂連環」﹐一斧快過一斧地斬向$n的$l" ]),
        ([ "attack": 6, "defense": 3, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N身形一矮從$n的兵刃底下鑽了出來﹐「疾電穿雲」斧刃自下而上撩向$n的$l" ]),
        ([ "attack": 5, "defense": 2, "force": 2, "bonus": 10, "damage_type": "割傷",
           "action": "$N縱身躍起把斧頭高舉過頂﹐這一記「雷霆萬鈞」快如電閃直剁$n的$l" ]),
    });

    /* 學成：累積到 20 級門檻時 gain，直接練成 20 級；學成那一級起每級給武術造詣 等級 × 10。 */
    art_entry        = 20;
    art_entry_reward = 1;
    art_entry_msg    = "你的迅雷斧法已有小成。\n";

    /* 每級武術造詣 等級 × 10；41 級起武學之道 (等級 − 40) × 10。 */
    ma_coef  = 10;
    mm_start = 41;
    mm_minus = 40;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("soonlight axe");
    setup();
}

/* 可用在單手斧與左手斧。 */
int valid_enable(string usage) { return usage == "axe" || usage == "secondhand axe"; }

/* 右手單手斧、左手左手斧，兩邊都用迅雷斧法。 */
int dual_axes(object me)
{
    return objectp(me->query_temp("weapon/axe"))
        && objectp(me->query_temp("weapon/secondhand axe"))
        && me->skill_mapped("axe") == "soonlight axe"
        && me->skill_mapped("secondhand axe") == "soonlight axe";
}

/* 連擊：機率 行動力÷3 %（最高 50%），多砍 1 + 行動力÷100 斧；
 * 傷害與一般招式相同，耗精為一般招式的 120%，自身不停頓。 */
void thunder_combo(object me, object opponent)
{
    object *axes;
    int move, hits, i;

    move = me->query_ability("move");
    if( random(100) >= (move / 3 > 50 ? 50 : move / 3) ) return;

    hits = 1 + move / 100;
    axes = ({ me->query_temp("weapon/axe"), me->query_temp("weapon/secondhand axe") });
    message_vision("\n" HIY "$N雙斧越使越快﹐斧光連成一片﹐一斧接一斧地劈向$n﹗\n\n" NOR,
        me, opponent, 1);
    for( i = 0; i < hits; i++ ) {
        if( !objectp(opponent) || !living(me) || environment(me) != environment(opponent) ) break;
        special_hit(me, opponent, axes[i % 2], 0, 100, 120);
    }
}

/* 雙手各砍一斧之後（同一回合的第二斧）判定連擊；迅雷斧法 90 級起。 */
void attack_using(object me, object opponent, object weapon)
{
    int t;

    ::attack_using(me, opponent, weapon);
    if( !objectp(opponent) || !objectp(me) || !living(me) ) return;
    if( me->query_skill("soonlight axe", 1) < 90 || !dual_axes(me) ) return;

    t = time();
    if( me->query_temp("soonlight_axe/round") != t ) {
        me->set_temp("soonlight_axe/round", t);
        return;
    }
    me->delete_temp("soonlight_axe/round");
    if( environment(me) != environment(opponent) ) return;
    thunder_combo(me, opponent);
}
