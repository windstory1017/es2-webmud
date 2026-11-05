/*---
description: 風嘯拳法（NEW 新版武功寫法）。共通徒手武功，五堂鎮雍家的家傳拳法，
             武道世家打根基的基礎拳法：招式不華麗但扎實，練成後給的武學之道較高。
             特攻：90 級起出拳前有機會吐氣調息，自己回精、對手停頓。
             設計見 docs/martial_arts/共通_風嘯拳法.md；任務見 d/wutang/npc/yung_tai.c。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

#define SPECIAL_LEVEL 90        // 特攻從幾級起出現
#define SPECIAL_GIN   50        // 特攻回復的精

private void create()
{
    seteuid(getuid());

    art_id    = "fonxiaoken";
    art_name  = "風嘯拳法";
    art_usage = "unarmed";
    art_desc  = "五堂鎮雍家的家傳拳法，武道世家用來打熬根基的功夫。"
                + "馬步、直拳、肘撞，一招一式都不華麗，卻拳拳紮實。";
    art_bonus = 0;

    moves = ({
        ([ "name": "平地起風", "attack": 6, "defense": 4, "force": 1, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N兩腳穩穩紮成馬步﹐腰胯一擰﹐一招「平地起風」右拳由腰間直衝$n的$l" ]),
        ([ "attack": 5, "defense": 3, "force": 2, "bonus": 5, "damage_type": "瘀傷",
           "action": "$N左拳虛晃﹐右拳緊跟著從肩後掄了過來﹐呼的一聲砸向$n的$l" ]),
        ([ "name": "順風推舟", "attack": 5, "defense": 5, "force": 1, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N踏前半步﹐使出「順風推舟」﹐雙掌並肩齊出沉沉地推向$n的$l" ]),
        ([ "attack": 6, "defense": 3, "force": 2, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N沉肩墜肘﹐一個箭步搶進中門﹐右肘貼著身子橫撞$n的$l" ]),
        ([ "name": "逆風穿林", "attack": 6, "defense": 4, "force": 1, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N側身斜跨左臂向外一格﹐正是「逆風穿林」﹐右拳自肘底穿出搗向$n的$l" ]),
        ([ "name": "風嘯長空", "attack": 5, "defense": 3, "force": 2, "bonus": 5, "damage_type": "瘀傷",
           "action": "$N雙拳一收一放﹐拳風呼呼作響﹐「風嘯長空」一連三拳錘向$n的$l" ]),
    });

    /* 學成：累積到 5 級門檻時 gain，直接練成 5 級；學成那一級起每級給獎勵。 */
    art_entry        = 5;
    art_entry_reward = 1;
    art_entry_msg    = "你練成了風嘯拳法。\n";

    /* 每級武術造詣 等級 × 10；21 級起武學之道 (等級 − 20) × 15。 */
    ma_coef  = 10;
    mm_start = 21;
    mm_minus = 20;
    mm_coef  = 15;

    DAEMON_D->register_skill_daemon("fonxiaoken");
    setup();
}

/* 學習條件：無，所有職業都能學（使用者指示，2026-11-05）；只能從雍泰的任務學到。 */

/* 特攻機率（千分比）：90 級 10%，200 級 15%，中間依等級增加。 */
int special_chance(object me)
{
    int lv = me->query_skill(art_id, 1);

    if( lv < SPECIAL_LEVEL ) return 0;
    if( lv > 200 ) lv = 200;
    return 100 + 50 * (lv - SPECIAL_LEVEL) / (200 - SPECIAL_LEVEL);
}

/* 出拳前判定特攻（不必命中）：不造成傷害，自己回精，對手停頓 1 回合，自己不停頓；
 * 之後照常出這一拳。 */
void attack_using(object me, object opponent, object weapon)
{
    if( !opponent ) return;
    if( random(1000) < special_chance(me) ) {
        message_vision(HIW "$N吐氣開聲﹐馬步一沉調勻了呼吸﹐$n被這股拳勢逼得一時緩不過手來。\n" NOR,
            me, opponent, 1);
        me->supplement_stat("gin", SPECIAL_GIN);
        opponent->start_busy(1);
    }
    ::attack_using(me, opponent, weapon);
}
