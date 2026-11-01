/*---
description: 步玄心法（NEW 新版武功寫法）。步玄派內功，enable 在 force 上使用。
             exert myst-force（修習）／refresh（回精）／balance（調息）。120 級大成時屬性提昇並全服公告。
             設計見 docs/martial_arts/步玄派_門派設計.md。
---*/
#include <ansi.h>

inherit "/std/martial_art";

#define GRAND_LEVEL 120     // 大成

/* 大成時提昇的屬性：慧根（wis）以外全部 +1（使用者定案，2026-11-01）。 */
private string *grand_attrs = ({ "str", "cor", "int", "spi", "cps", "dex", "con" });
private mapping attr_names = ([
    "str": "膂力", "cor": "膽識", "int": "悟性", "spi": "靈性",
    "cps": "定力", "dex": "機敏", "con": "根骨",
]);

private void create()
{
    seteuid(getuid());

    art_id    = "myst-force";
    art_name  = "步玄心法";
    art_usage = "force";
    art_desc  = "步玄派的內功心法，清和綿長，能調息回復精氣；"
                + "大成之時氣血精神渾身舒暢，周身資質更上一層。";

    art_entry = 5;

    // 升級獎勵（使用者定案，2026-11-01）：學成之後每級武術造詣 等級 × 10；
    // 武學之道 (等級 − 31) × 10，32 級起。
    ma_coef  = 10;
    mm_start = 32;
    mm_minus = 31;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("myst-force");
    setup();
}

/* 學習條件：人物等級 5、步玄派弟子。 */
int valid_learn(object me)
{
    if( !is_faction(me, "scholar.myst") )
        return notify_fail("你不是步玄派弟子﹐無法修練步玄心法。\n");
    if( me->query_level() < 5 )
        return notify_fail("你的根基尚淺﹐還領會不了步玄心法的要旨。\n");
    return 1;
}

/* 內功不會拿來出招。 */
void attack_using(object me, object opponent, object weapon) { }

/* 學成：累積到 5 級門檻，gain 時直接練成 5 級。 */
void skill_completed(object me, string skill)
{
    tell_object(me, HIW "你的步玄心法已有小成﹐一股清和的真氣在丹田緩緩流轉。\n" NOR);
}

/* 大成：屬性提昇（只給一次）並全服公告。
 * 公告比照改版前讚揚的全服黃字：沒有頻道標籤、不走 event 頻道，玩家無法關掉
 * （使用者定案，2026-11-01）。 */
private void grand_mastery(object me)
{
    string attr;

    me->set("myst/grand_mastery", 1);
    tell_object(me, HIY "你的步玄心法終於大成，只覺氣血精神渾身舒暢，忍不住仰天發出一聲長嘯！\n" NOR);
    message("system",
        HIY + me->name() + "的步玄心法終於大成，只覺氣血精神渾身舒暢，"
        + "忍不住仰天長嘯，嘯聲綿延數十里，久久不絕！\n" NOR,
        users(), ({ me }));

    foreach(attr in grand_attrs) {
        me->set_attr(attr, me->query_attr(attr, 1) + 1);
        tell_object(me, HIY "你的" + attr_names[attr] + "提高了！\n" NOR);
    }
}

/* 每級獎勵（引擎）；超過學成等級後每升一級精、氣、神上限 +3／+3／+2。 */
void skill_advanced(object me, string skill)
{
    int lv;

    ::skill_advanced(me, skill);

    if( !userp(me) ) return;
    lv = me->query_skill("myst-force", 1);
    if( lv > 5 ) {
        me->advance_stat("gin", 3);
        me->advance_stat("kee", 3);
        me->advance_stat("sen", 2);
    }

    if( lv >= GRAND_LEVEL && !me->query("myst/grand_mastery") )
        grand_mastery(me);
}

/* 修習：每回合消耗精、氣各 2（受傷減少上限各 1），
 * 得到 基本公式 + random(悟性) 的步玄心法點數，同時累積基本內功。 */
int do_exercise(object me)
{
    if( me->query_stat("kee") < 3 || me->query_stat("gin") < 3 ) {
        tell_object(me, "你覺得氣血浮動、精力難繼﹐已無法再運轉步玄心法。\n");
        return 0;
    }

    me->consume_stat("kee", 2);
    me->damage_stat("kee", 1);
    me->consume_stat("gin", 2);
    me->damage_stat("gin", 1);

    me->improve_restored_force_tick();
    me->improve_skill_exact("myst-force",
        COMBAT_D->int_formula_exp(me)
        + random(me->query_attr("int")));
    return 1;
}

int halt_exercise(object me, object owner, object from, string how)
{
    return 1;
}

/* balance（調息）：消耗神 10 + 步玄心法/20；回復精 機敏 + 步玄心法/2、
 * 氣 根骨 + 步玄心法/4（只回復現在值）；戰鬥中可用，自身停頓 1 回合
 * （使用者定案，2026-11-01）。 */
int do_balance(object me)
{
    int sk, cost;

    sk = me->query_skill("myst-force", 1);
    cost = 10 + sk / 20;
    if( me->query_stat("sen") <= cost )
        return notify_fail("你的心神不足﹐調勻不了體內的步玄真氣。\n");
    if( me->query_stat("gin") >= me->query_stat_effective("gin")
    &&  me->query_stat("kee") >= me->query_stat_effective("kee") )
        return notify_fail("你的精氣充盈﹐用不著調息。\n");

    me->consume_stat("sen", cost);
    me->supplement_stat("gin", me->query_attr("dex") + sk / 2);
    me->supplement_stat("kee", me->query_attr("con") + sk / 4);
    message_vision("$N暗運步玄心法﹐一口真氣周流百骸﹐精神氣力漸漸平復了過來。\n", me);
    me->start_busy(1);
    return 1;
}

/* refresh（回精）：回復精 步玄心法 + 10（只回復現在值）；
 * 消耗氣 步玄心法/20，自身停頓 1 回合，比照封山派內功（使用者定案，2026-11-01：回復量 = 等級 + 10）。 */
int do_refresh(object me)
{
    int sk, cost;

    sk = me->query_skill("myst-force", 1);
    cost = sk / 20;
    if( me->query_stat("kee") <= cost )
        return notify_fail("你的氣不夠﹐無法運起步玄心法回復精神。\n");
    if( me->query_stat("gin") >= me->query_stat_effective("gin") )
        return notify_fail("你的精神飽滿﹐用不著運功。\n");

    if( cost > 0 ) me->consume_stat("kee", cost);
    me->supplement_stat("gin", sk + 10);
    message_vision("$N運起步玄心法﹐一股清和的真氣直透靈台﹐精神為之一振。\n", me);
    me->start_busy(1);
    return 1;
}

varargs int exert_function(object me, string func, object target)
{
    switch(func) {
    case "myst-force":
        message_vision(HIY "$N盤膝坐定﹐默運步玄心法﹐一股清和的真氣在週身經脈緩緩流轉。\n" NOR, me);
        me->start_busy((: do_exercise, me :), (: halt_exercise, me :));
        return 1;
    case "refresh":
        return do_refresh(me);
    case "balance":
        return do_balance(me);
    default:
        return notify_fail("步玄心法沒有這種功能。\n");
    }
}
