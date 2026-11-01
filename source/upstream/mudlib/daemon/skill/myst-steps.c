/*---
description: 步玄七絕。步玄派獨門輕功，七式閃避取自駱步玄與風神族夜鷹比輕功、
             沐華居大鷹字畫的典故；閃避機率照 ES2 原本公式。
             120 級起被攻擊時有機會觸發逍遙遊，回復精、氣的現在值。
             設計見 docs/martial_arts/步玄派_門派設計.md。
---*/
#include <ansi.h>
inherit SKILL;

#define XIAOYAO_LEVEL   120     // 逍遙遊的門檻
#define XIAOYAO_MAX     25      // 觸發機率上限（%）
#define XIAOYAO_CD      4       // 冷卻 2 tick（一 tick 兩秒）

/* 閃避敘述：接在對方出招敘述後面，只有閃過時才出現。
 * $N 是出招的人，$n 是閃避的人。 */
string *dodge_actions = ({
    "$n足下輕輕一錯﹐使出「一步之差」﹐$N這一招堪堪擦身而過",
    "$n雙袖一振如鷹展翅﹐「振翮凌雲」拔地而起﹐讓$N撲了個空",
    "$n身形隨著$N的來勢飄開﹐正是一式「逐風無跡」﹐輕輕巧巧地避了開去",
    "$n腳下斜斜滑開半步﹐一招「回風轉步」繞到$N身後﹐讓這一招落了個空",
    "$n一招「夜鷹投林」﹐身形向後倒掠數尺﹐避開了這招",
    "$n袍袖一拂﹐「青衫不染」從$N招底飄然穿過﹐連衣角都沒讓碰著",
    "$n顧盼之間身形左晃右閃﹐一式「顧盼生風」﹐讓$N的攻勢盡數落空",
});

void xiaoyao(object me);

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("myst-steps");
    setup();
}

int valid_enable(string usage)
{
    return usage == "dodge";
}

/* 學習條件：步玄派弟子。 */
int valid_learn(object me)
{
    if( me->query("custom_faction") != "scholar.myst" )
        return notify_fail("你不是步玄派弟子﹐無法修練步玄七絕。\n");
    return 1;
}

// 學成：照一般門檻累積到 10 級時 gain，直接練成 10 級。
int query_entry_level() { return 10; }

void skill_completed(object me, string sk)
{
    tell_object(me, HIY "你終於參透了步玄七絕的步法﹐身形一展﹐已隱隱有鷹揚之勢。\n" NOR);
}

/* 每級獎勵（超過學成等級才給，使用者定案，2026-11-01）：武術造詣 等級 × 10；
 * 41 級起武學之道 (等級 − 40) × 10。 */
void skill_advanced(object me, string sk)
{
    int lv;

    if( !userp(me) ) return;
    lv = me->query_skill("myst-steps", 1);
    if( lv <= 10 ) return;
    me->gain_score("martial art", lv * 10);
    if( lv > 40 ) me->gain_score("martial mastery", (lv - 40) * 10);
}

/*
 * 閃避機率照 ES2 原本的公式，步法只提供有效閃避值。
 * 這裡先選好一句閃避敘述交給出招的一方；戰鬥程式只有在真的閃過時
 * 才會把這句接到出招敘述後面。
 */
int dodge_using(object me, int ability, int strength, object from)
{
    object attacker;

    if( objectp(me) && objectp(from) ) {
        attacker = from;
        if( !from->is_character()
        &&  objectp(environment(from)) && environment(from)->is_character() )
            attacker = environment(from);

        if( attacker->is_character() && attacker != me ) {
            attacker->set_temp("defend_message",
                dodge_actions[random(sizeof(dodge_actions))]);
            xiaoyao(me);
        }
    }

    return me->query_skill("dodge");
}

/* 逍遙遊：步玄七絕 120 級起，被攻擊時（不論是否閃過）有 悟性/2 %（最多 25%）的機會觸發；
 * 冷卻 2 tick。回復精 = 步玄七絕/2、氣 = 步玄七絕/3，只回復現在值，不回復受傷減少的上限
 * （使用者定案，2026-11-01）。只顯示一行敘述，不打出武功或特效的名稱。 */
void xiaoyao(object me)
{
    int sk, chance;

    sk = me->query_skill("myst-steps", 1);
    if( sk < XIAOYAO_LEVEL ) return;
    if( me->query_temp("myst/xiaoyao_next") > time() ) return;

    chance = me->query_attr("int") / 2;
    if( chance > XIAOYAO_MAX ) chance = XIAOYAO_MAX;
    if( random(100) >= chance ) return;

    me->set_temp("myst/xiaoyao_next", time() + XIAOYAO_CD);
    me->supplement_stat("gin", sk / 2);
    me->supplement_stat("kee", sk / 3);
    message_vision(HIY "$N步履一鬆﹐身形似風中新枝般舒展開來﹐精神氣力為之一振。\n" NOR, me);
}
