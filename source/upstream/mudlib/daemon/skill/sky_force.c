/*---
description: 玄天心法（NEW 新版武功寫法）。玄天派內功，enable 在 force 上使用。
             exert sky force（修習）／refresh（回精）／heal（療傷）。100 級起每 20 級悟性 +1、精氣神上限各 +10。
             設計見 docs/martial_arts/玄天教_門派設計.md。
---*/
#include <ansi.h>

inherit "/std/martial_art";

#define FACTION "scholar.sky"

/* 境界獎勵（使用者定案，2026-11-14）：100 級起（含 100 級）每 20 級（100、120、140 … 級）
 * 悟性 +1、精氣神上限各 +10。領過幾次記在 sky_force/milestones，不會重複給。 */
#define MILESTONE_FIRST 100
#define MILESTONE_STEP  20
#define MILESTONE_STAT  10

private void create()
{
    seteuid(getuid());

    art_id    = "sky force";
    art_name  = "玄天心法";
    art_usage = "force";
    art_desc  = "玄天派的內功心法，沉靜內斂，能運氣回復精神、調理傷勢；"
                + "修為日深，心神愈發澄明。";

    art_entry = 10;

    // 升級獎勵（使用者定案，2026-11-14）：學成之後每級武術造詣 等級 × 10；
    // 武學之道 (等級 - 50) × 10，51 級起。
    ma_coef  = 10;
    mm_start = 51;
    mm_minus = 50;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("sky force");
    setup();
}

/* 學習條件：人物等級 5、玄天派弟子（使用者定案，2026-11-14）。 */
int valid_learn(object me)
{
    if( !is_faction(me, FACTION) )
        return notify_fail("你不是玄天派弟子﹐無法修練玄天心法。\n");
    if( me->query_level() < 5 )
        return notify_fail("你的根基尚淺﹐還領會不了玄天心法的要旨。\n");
    return 1;
}

/* 內功不會拿來出招。 */
void attack_using(object me, object opponent, object weapon) { }

/* 學成：累積到 10 級門檻，gain 時直接練成 10 級。 */
void skill_completed(object me, string skill)
{
    tell_object(me, HIW "你的玄天心法已有小成﹐一縷沉靜的真氣在丹田緩緩流轉。\n" NOR);
}

private void give_milestones(object me, int lv)
{
    int n, done, steps;

    if( lv < MILESTONE_FIRST ) return;
    n = (lv - MILESTONE_FIRST) / MILESTONE_STEP + 1;
    done = me->query("sky_force/milestones");
    if( n <= done ) return;

    steps = n - done;
    me->set("sky_force/milestones", n);
    me->set_attr("int", me->query_attr("int", 1) + steps);
    me->advance_stat("gin", MILESTONE_STAT * steps);
    me->advance_stat("kee", MILESTONE_STAT * steps);
    me->advance_stat("sen", MILESTONE_STAT * steps);
    tell_object(me, HIY "你的玄天心法又進一層﹐心神益發澄明。\n"
        "你的悟性增加了。\n"
        "你的精、氣、神因為修煉玄天心法增進不少。\n" NOR);
}

/* 每級獎勵（引擎）；超過學成等級後每升一級：精上限 +2～3、氣上限 +2～3、神上限 +1～2；
 * 再加上 100 級起每 20 級的境界獎勵（使用者定案，2026-11-14）。 */
void skill_advanced(object me, string skill)
{
    int lv;

    ::skill_advanced(me, skill);

    if( !userp(me) ) return;
    lv = me->query_skill("sky force", 1);
    if( lv > 10 ) {
        me->advance_stat("gin", 2 + random(2));
        me->advance_stat("kee", 2 + random(2));
        me->advance_stat("sen", 1 + random(2));
    }
    give_milestones(me, lv);
}

/* 修習：每回合消耗精、氣各 2（受傷減少上限各 1），
 * 得到 基本公式 + random(悟性) 的玄天心法點數，同時累積基本內功。 */
int do_exercise(object me)
{
    if( me->query_stat("kee") < 3 || me->query_stat("gin") < 3 ) {
        tell_object(me, "你覺得氣血浮動、精力難繼﹐已無法再運轉玄天心法。\n");
        return 0;
    }

    me->consume_stat("kee", 2);
    me->damage_stat("kee", 1);
    me->consume_stat("gin", 2);
    me->damage_stat("gin", 1);

    me->improve_restored_force_tick();
    me->improve_skill_exact("sky force",
        COMBAT_D->int_formula_exp(me)
        + random(me->query_attr("int")));
    return 1;
}

int halt_exercise(object me, object owner, object from, string how)
{
    return 1;
}

/* refresh（回精）：消耗氣 玄天心法/6，回復精的現在值 玄天心法；
 * 戰鬥中可用，自身停頓 1 回合（使用者定案，2026-11-14）。 */
int do_refresh(object me)
{
    int sk, cost;

    sk = me->query_skill("sky force", 1);
    cost = sk / 6;
    if( me->query_stat("kee") <= cost )
        return notify_fail("你的氣不夠﹐無法運起玄天心法回復精神。\n");
    if( me->query_stat("gin") >= me->query_stat_effective("gin") )
        return notify_fail("你的精神飽滿﹐用不著運功。\n");

    if( cost > 0 ) me->consume_stat("kee", cost);
    me->supplement_stat("gin", sk);
    message_vision("$N運起玄天心法﹐一縷沉靜的真氣直透靈台﹐精神為之一振。\n", me);
    me->start_busy(1);
    return 1;
}

/* heal（療傷）：消耗精 玄天心法/20，回復氣的現在值 5 + 玄天心法/2；
 * 戰鬥中可用，自身停頓 1 回合（使用者定案，2026-11-14）。 */
int do_heal(object me)
{
    int sk, cost;

    sk = me->query_skill("sky force", 1);
    cost = sk / 20;
    if( me->query_stat("gin") <= cost )
        return notify_fail("你的精力不足﹐無法運起玄天心法療傷。\n");
    if( me->query_stat("kee") >= me->query_stat_effective("kee") )
        return notify_fail("你的氣血充盈﹐用不著療傷。\n");

    if( cost > 0 ) me->consume_stat("gin", cost);
    me->supplement_stat("kee", 5 + sk / 2);
    message_vision("$N運起玄天心法﹐一縷沉靜的真氣緩緩流過經脈﹐傷勢漸漸平復。\n", me);
    me->start_busy(1);
    return 1;
}

varargs int exert_function(object me, string func, object target)
{
    switch(func) {
    case "sky force":
        message_vision(HIY "$N盤膝坐定﹐默運玄天心法﹐一縷沉靜的真氣在週身經脈緩緩流轉。\n" NOR, me);
        me->start_busy((: do_exercise, me :), (: halt_exercise, me :));
        return 1;
    case "refresh":
        return do_refresh(me);
    case "heal":
        return do_heal(me);
    default:
        return notify_fail("玄天心法沒有這種功能。\n");
    }
}
