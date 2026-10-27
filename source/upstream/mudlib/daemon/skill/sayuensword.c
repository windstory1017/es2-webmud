/*---
description: 水雲劍法（NEW 新版武功寫法）。共通劍法，劍意取水雲無常形；四招，沒有特技。
             設計見 docs/martial_arts/共通_水雲劍法.md。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

private void create()
{
    seteuid(getuid());

    art_id    = "sayuensword";
    art_name  = "水雲劍法";
    art_usage = "sword";
    art_desc  = "一套流傳於市井的劍法，劍意取水雲沒有定形，架式飄逸好看，"
                + "只是使來徒具其形，勁道總差了幾分。";
    art_bonus = 0;

    moves = ({
        ([ "name": "行雲流水", "attack": 5, "defense": 3, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N手中$w隨手一揮﹐使出「行雲流水」﹐劍勢飄來蕩去頗為瀟灑﹐遞到$n的$l時卻輕飄飄的" ]),
        ([ "name": "雲捲雲舒", "attack": 4, "defense": 4, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N學著雲氣舒捲的樣子﹐劍尖慢慢畫了幾個圈子﹐架式擺得十足﹐這才點向$n的$l" ]),
        ([ "name": "水無常形", "attack": 5, "defense": 3, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N腳下一晃﹐劍招忽左忽右沒個定準﹐倒有幾分「水無常形」的味道﹐只是力道虛浮﹐斜削$n的$l" ]),
        ([ "name": "浪疊千重", "attack": 6, "defense": 2, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N一招「浪疊千重」連刺數劍﹐劍光層層湧來聲勢唬人﹐落到$n的$l時已沒剩幾分勁道" ]),
    });

    /* 學成：累積到 5 級門檻時 gain，直接練成 5 級；學成那一級起每級給武術造詣 等級 × 10。 */
    art_entry        = 5;
    art_entry_reward = 1;
    art_entry_msg    = "你把水雲劍法的四招記熟了﹐使起來總算有幾分樣子。\n";

    /* 每級武術造詣 等級 × 10；31 級起武學之道 (等級 − 30) × 10（30 級以上要先習得精義）。 */
    ma_coef  = 10;
    mm_start = 31;
    mm_minus = 30;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("sayuensword");
    setup();
}

/* 習得水雲劍法精義後的四招：招名不變，改為有實質威力的敘述，數值小幅提升（使用者定案，2026-10-20）。 */
mapping *ESSENCE_MOVES = ({
    ([ "name": "行雲流水", "attack": 6, "defense": 4, "force": 1, "bonus": 5, "damage_type": "刺傷",
       "action": "$N手中$w如行雲流水般連綿遞出﹐劍勢看似隨意﹐每一劍卻都暗藏後勁﹐直取$n的$l" ]),
    ([ "name": "雲捲雲舒", "attack": 6, "defense": 5, "force": 1, "bonus": 5, "damage_type": "刺傷",
       "action": "$N劍尖畫圈﹐劍光如雲氣層層捲起﹐封住$n的退路﹐隨即一劍點向$n的$l" ]),
    ([ "name": "水無常形", "attack": 7, "defense": 3, "force": 1, "bonus": 5, "damage_type": "割傷",
       "action": "$N腳下一晃﹐劍招忽左忽右無跡可尋﹐「水無常形」使到妙處﹐劍鋒已斜削到$n的$l" ]),
    ([ "name": "浪疊千重", "attack": 7, "defense": 3, "force": 2, "bonus": 5, "damage_type": "刺傷",
       "action": "$N一招「浪疊千重」連刺數劍﹐劍光一浪高過一浪﹐後勁直透$n的$l" ]),
});

int has_essence(object me)
{
    return objectp(me) && me->query("sayuensword/essence");
}

/* 出招：習得精義後改用精義版的四招。 */
mapping *usable_moves(object me)
{
    if( has_essence(me) ) return ESSENCE_MOVES;
    return ::usable_moves(me);
}

/* 水雲劍法得到點數時，習得精義的人另外替精義擲一次同樣的公式（各拿各的）。 */
void skill_improved(object me, string skill)
{
    if( skill != "sayuensword" || !userp(me) || !has_essence(me) ) return;
    me->improve_skill_exact("sayuensword essence", COMBAT_D->int_formula_exp(me));
}

/* 水雲劍法與精義都到 90 級：每次打中，有 水雲劍法等級/600 的機率回復 水雲劍法等級/3 的精。 */
void after_strike(object me, object opponent, object weapon, mapping m, int damage)
{
    int sk;

    if( damage <= 0 || !has_essence(me) ) return;
    sk = me->query_skill("sayuensword", 1);
    if( sk < 90 || me->query_skill("sayuensword essence", 1) < 90 ) return;
    if( random(600) >= sk ) return;
    me->supplement_stat("gin", sk / 3);
    message_vision("$N劍勢一收一放之間﹐氣息隨之流轉﹐精神為之一振。\n", me);
}

/* 只能用在劍術。 */
int valid_enable(string usage) { return usage == "sword"; }

/* 等級上限（使用者定案，2026-10-20）：暫時 30；習得水雲劍法精義後才能往上練（精義尚未製作）。 */
int query_skill_cap(object me)
{
    return has_essence(me) ? 200 : 30;
}
