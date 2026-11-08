/*---
description: 蝶影幻步。盜賊的閃躲步法，輕盈如蝶；90 級起可用 perform butterfly-steps:disappear
             施展「蝶影迷蹤步」，必定脫離戰鬥並進入潛行狀態。
             設計見 docs/martial_arts/瑯夷派_蝶影幻步.md；任務見 d/wutang/npc/yaesae.c。
---*/
#include <ansi.h>
inherit SKILL;

#define DISAPPEAR_LEVEL   90        // 蝶影迷蹤步從幾級起可用
#define DISAPPEAR_GIN     100       // 消耗的精（目前值）
#define DISAPPEAR_FATIGUE 10        // 累積的疲勞

/* 閃避敘述：接在對方出招敘述後面，只有閃過時才出現。
 * $N 是出招的人，$n 是閃避的人。 */
string *dodge_actions = ({
    "$n身子輕輕一飄﹐像蝶翼振了一下﹐讓$N撲了個空",
    "$n足尖一點﹐順著勁風向旁滑開﹐$N的攻勢擦身而過",
    "$n腰身一折﹐貼著地面斜掠出去﹐避開了這招",
    "$n左一晃右一閃﹐幾個折步之間已繞出丈許﹐$N只抓到一片衣角",
    "$n翩然往後掠起﹐落地時悄無聲息﹐這一招落了個空",
    "$n身形一虛一實﹐留下一道淡淡的殘影﹐真身卻已站在$N的側面",
});

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("butterfly-steps");
    setup();
}

int valid_enable(string usage)
{
    return usage == "dodge";
}

/* 學習條件：盜賊（使用者定案，2026-11-07：盜賊都可學）；取得靠五堂鎮葉翔的任務。 */
int valid_learn(object me)
{
    if( me->query_class() != "thief" )
        return notify_fail("蝶影幻步只有盜賊才能修練。\n");
    return 1;
}

// 學成：照一般門檻累積到 10 級時 gain，直接練成 10 級。
int query_entry_level() { return 10; }

void skill_completed(object me, string sk)
{
    tell_object(me, HIY "你終於練成蝶影幻步。\n" NOR);
}

/* 每級獎勵（超過學成等級才給）：武術造詣 等級 × 10；41 級起武學之道 (等級 - 40) × 10。 */
void skill_advanced(object me, string sk)
{
    int lv;

    if( !userp(me) ) return;
    lv = me->query_skill("butterfly-steps", 1);
    if( lv <= 10 ) return;
    me->gain_score("martial art", lv * 10);
    if( lv > 40 ) me->gain_score("martial mastery", (lv - 40) * 10);
}

/* 閃避機率照 ES2 原本的公式，步法只提供有效閃躲值；這裡先選好一句閃避敘述。 */
int dodge_using(object me, int ability, int strength, object from)
{
    object attacker;

    if( objectp(me) && objectp(from) ) {
        attacker = from;
        if( !from->is_character()
        &&  objectp(environment(from)) && environment(from)->is_character() )
            attacker = environment(from);

        if( attacker->is_character() && attacker != me )
            attacker->set_temp("defend_message",
                dodge_actions[random(sizeof(dodge_actions))]);
    }

    return me->query_skill("dodge");
}

/* 蝶影迷蹤步：90 級起；消耗精目前值 100、疲勞 +10、自身不停頓；
 * 使出後必定脫離戰鬥並進入潛行狀態（隱藏值同 sneak 指令：1 + random(潛行 × 2)）。
 * 做法同茅山鬼煙術（taoism_of_darkness.c 的 do_smoke）。 */
int perform_action(object me, string act, object target)
{
    object *enemies;
    int sk, sneak;

    if( act != "disappear" ) return notify_fail("蝶影幻步沒有這種絕招。\n");

    enemies = filter(me->query_enemy(), (: objectp($1) :));
    if( !sizeof(enemies) ) return notify_fail("你並未與人交手﹐用不著脫身。\n");

    sk = me->query_skill("butterfly-steps", 1);
    if( sk < DISAPPEAR_LEVEL )
        return notify_fail("你的蝶影幻步火候未到﹐還無法施展蝶影迷蹤步。\n");
    if( me->query_stat("gin") < DISAPPEAR_GIN )
        return notify_fail("你精神耗損過甚﹐施展不了蝶影迷蹤步。\n");
    if( me->query_stat("fatigue") >= me->query_stat_maximum("fatigue") )
        return notify_fail("你筋疲力盡﹐實在跳不動了。\n");

    me->consume_stat("gin", DISAPPEAR_GIN);
    me->supplement_stat("fatigue", DISAPPEAR_FATIGUE);

    message_vision(HIW "$N使出「蝶影迷蹤步」﹐身形宛如蝶舞花叢﹐轉眼間消失不見。\n" NOR, me);

    // 雙方都停手（含殺意）；看得穿的對手之後可以重新出手。
    me->remove_all_killer();
    foreach(object ob in enemies)
        if( objectp(ob) ) ob->remove_killer(me);
    sneak = me->query_skill("sneak");
    me->set_temp("pending/hidden", 1 + (sneak > 0 ? random(sneak * 2) : 0));
    return 1;
}
