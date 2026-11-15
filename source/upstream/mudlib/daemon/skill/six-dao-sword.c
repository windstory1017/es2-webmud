/*---
description: 六陰絕識劍（NEW 新版武功寫法）。雪吟莊進階的左手劍法，可 enable 在副手劍。
             設計見 docs/martial_arts/左手劍法_六陰絕識劍.md。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

#define FACTION        "fighter.snow"
#define SPECIAL_LEVEL  90    // 特攻習得等級
#define SPECIAL_CHANCE 30    // 特攻機率 (%)，固定值

private void create()
{
    seteuid(getuid());

    art_id    = "six-dao-sword";
    art_name  = "六陰絕識劍";
    art_usage = "secondhand sword";
    art_desc  = "雪吟莊進階的左手劍法，劍意空靈、不帶殺意；火候到了，"
                + "出招命中後能施展【天譴劍式】搶攻，落空時能施展【血噬劍式】追擊。";
    art_bonus = 20;     // 強度為一般武功的 120%

    moves = ({
        ([ "name": "絕仁棄義", "attack": 9, "defense": 2, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N目視無物心漠然﹐一式「絕仁棄義」似巧若拙地穿過$n﹐手中$w殺意全無地直指$n的$l" ]),
        ([ "name": "絕學無憂", "attack": 9, "defense": 2, "force": 1, "bonus": 5, "damage_type": "割傷",
           "action": "$N看破$n招式死角﹐一式「絕學無憂」劍尖化為六道寒光﹐直指$n週身要害" ]),
        ([ "name": "絕聖棄智", "attack": 8, "defense": 3, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N手中$w毫無章法地往前一遞﹐正暗合「絕聖棄智」之意﹐竟直逼$n的$l" ]),
        ([ "name": "絕巧棄利", "attack": 8, "defense": 3, "force": 2, "bonus": 5, "damage_type": "刺傷",
           "action": "$N左腕不抖不顫﹐一式「絕巧棄利」﹐$w平平淡淡地點向$n的$l" ]),
    });

    art_entry     = 20;
    art_entry_msg = "你終於練成了六陰絕識劍﹐劍意空靈﹐竟隱隱有了萬念皆絕之感。\n";
    level_msgs    = ([
        90: "你終於悟出六陰絕識劍中的【天譴劍式】與【血噬劍式】！\n",
    ]);

    ma_coef  = 10;
    mm_start = 30;
    mm_minus = 29;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("six-dao-sword");
    setup();
}

/* 只能裝在副手劍。 */
int valid_enable(string usage)
{
    return usage == "secondhand sword";
}

/* 學習條件：雪吟莊弟子（進階技能，目前尚未開放任務學習，沒有師父傳授）。 */
int valid_learn(object me)
{
    if( !is_faction(me, FACTION) )
        return notify_fail("你不是雪吟莊弟子﹐無法修練六陰絕識劍。\n");
    return 1;
}

/* 天譴劍式：這一招命中後，再搶攻一招（一般攻擊，可被閃躲、格擋；耗精 100%）。
 * 搶攻直接呼叫 special_hit()，不會再觸發特攻。 */
private void heaven_strike(object me, object victim, object weapon)
{
    message_vision(HIW "$N招式尚未使老﹐卻已看破$n的套路﹐突施「天譴劍式」攻其不備﹗\n" NOR,
        me, victim, 1);
    special_hit(me, victim, weapon, 0, 100, 100, 0);
}

/* 血噬劍式：這一招沒命中時，打出普攻 150% 的不可防禦傷害
 * （不能閃躲、格擋，防具照樣減傷；耗精 100%）。 */
private void blood_strike(object me, object victim, object weapon)
{
    message_vision(HIW "$N抓住$n招式之間的破綻﹐以「血噬劍式」貫穿$n週身經脈﹗\n" NOR,
        me, victim, 1);
    special_hit(me, victim, weapon, 0, 150, 100, HIT_MUST | HIT_SILENT);
    if( objectp(victim) ) show_status(victim, kee_ratio(victim));
}

void after_strike(object me, object opponent, object weapon, mapping m, int damage)
{
    if( !objectp(opponent) || environment(opponent) != environment(me) ) return;
    if( me->query_skill("six-dao-sword", 1) < SPECIAL_LEVEL ) return;
    if( random(100) >= SPECIAL_CHANCE ) return;

    if( damage > 0 ) heaven_strike(me, opponent, weapon);
    else blood_strike(me, opponent, weapon);
}
