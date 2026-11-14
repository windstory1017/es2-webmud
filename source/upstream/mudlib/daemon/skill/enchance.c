/*---
description: 強化之術（enchance）。劍甲門的法術類技能，安裝在 magic
             （enable magic with enchance），以 conjure 施展銀咒（silver mark）、
             金咒（gold mark），重新隨機決定武器的傷害力。
design: docs/martial_arts/劍甲門_強化之術.md（2026-11-14）
note: 兩種咒每次都消耗 20 點神，金咒另付 2.5 兩黃金；金咒不會損毀武器、不給技能經驗。
      銀咒非獨一無二的武器有機會損毀，機率 50% - 靈性/4 - 技能/5，最低 10%。
      銀咒的技能經驗用標準悟性公式（COMBAT_D->int_formula_exp）。
---*/

#include <ansi.h>
#include <combat.h>

inherit SKILL;

#define SKILL_ID    "enchance"
#define SEN_COST    20
#define GOLD_COST   25000      // 2.5 兩黃金（1 兩黃金 = 10000 文）
#define MIN_BREAK   10         // 損毀機率下限（％）

// 各類武器的強度上限：({ 傷害下限最高, 傷害上限最高, 力量修正上限(%), 乘數等級上限 })
private mapping limits = ([
    "axe":               ({ 15, 135, 200, 5 }),
    "secondhand axe":    ({ 15, 135, 200, 5 }),
    "blade":             ({ 14, 110, 200, 4 }),
    "secondhand blade":  ({ 14, 110, 200, 4 }),
    "blunt":             ({ 16, 100, 300, 6 }),
    "secondhand blunt":  ({ 16, 100, 300, 6 }),
    "dagger":            ({ 18,  75, 100, 3 }),
    "secondhand dagger": ({ 18,  75, 100, 3 }),
    "needle":            ({ 18,  75, 100, 3 }),
    "secondhand needle": ({ 18,  75, 100, 3 }),
    "sword":             ({ 15, 110, 150, 5 }),
    "secondhand sword":  ({ 15, 110, 150, 5 }),
    "twohanded axe":     ({ 15, 260, 300, 5 }),
    "twohanded blade":   ({ 14, 210, 300, 4 }),
    "twohanded blunt":   ({ 16, 190, 450, 6 }),
    "twohanded pike":    ({ 26, 176, 150, 6 }),
    "twohanded staff":   ({ 14, 158, 375, 4 }),
    "twohanded sword":   ({ 15, 210, 225, 5 }),
    "whip":              ({ 26, 110, 100, 6 }),
]);

void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon(SKILL_ID);
}

string type() { return "magic"; }

int valid_enable(string usage) { return usage == "magic"; }

// 銀咒損毀機率（％）：50 - 靈性/4 - 技能/5，最低 10。
int break_chance(object me)
{
    int chance;

    chance = 50 - me->query_attr("spi") / 4 - me->query_skill(SKILL_ID, 1) / 5;
    if( chance < MIN_BREAK ) chance = MIN_BREAK;
    return chance;
}

// 重骰一種武器技能的傷害力。顯示的傷害力為 下限＝roll＋乘數、上限＝roll＋乘數×range，
// 先抽乘數與下限，再反推 roll，最後在不超過上限的範圍內抽 range。
private class damage_parameter reroll(int *lim)
{
    class damage_parameter dp;
    int x, low, r, ymax;

    x = 1 + random(lim[3]);
    low = 1 + random(lim[0]);
    r = low - x;
    ymax = (lim[1] - r) / x;
    if( ymax < 1 ) ymax = 1;

    dp = new(class damage_parameter);
    dp->multipler = x;
    dp->range = 1 + random(ymax);
    dp->roll = r;
    dp->bonus = 1 + random(lim[2]);
    return dp;
}

// 這件東西是不是武器：有傷害力設定，而且至少有一種技能在強度表裡。
private int is_weapon(object ob)
{
    mapping dmg;
    string sk;

    dmg = ob->query("damage");
    if( !mapp(dmg) ) return 0;
    foreach(sk in keys(dmg))
        if( !undefinedp(limits[sk]) ) return 1;
    return 0;
}

// 武器的主要技能（顯示結果用）。
private string main_skill(object ob)
{
    mixed sk = ob->query("wield_as");

    if( arrayp(sk) ) sk = sizeof(sk) ? sk[0] : 0;
    if( stringp(sk) && classp(ob->query("damage/" + sk)) ) return sk;
    foreach(sk in keys(ob->query("damage")))
        if( classp(ob->query("damage/" + sk)) ) return sk;
    return 0;
}

int conjure_magic(object me, string spell, object target)
{
    string cname, name, sk, main;
    int gold;
    class damage_parameter dp;

    if( spell == "silver_mark" ) { cname = "銀咒"; gold = 0; }
    else if( spell == "gold_mark" ) { cname = "金咒"; gold = 1; }
    else return notify_fail("強化之術沒有這種法術。\n");

    if( !objectp(target) )
        return notify_fail("指令格式﹕conjure " + replace_string(spell, "_", " ")
            + " on <武器>\n");

    if( environment(target) != me || !is_weapon(target) )
        return notify_fail(gender_self(me) + "所施展的" + cname + "，只能施用在身上的兵刃上。\n");

    if( me->query_stat("sen") < SEN_COST )
        return notify_fail(gender_self(me) + "的「神」不足以施展這個法術。\n");
    if( gold && !me->can_afford(GOLD_COST) )
        return notify_fail(gender_self(me) + "身上的黃金不夠施展金咒。\n");

    me->consume_stat("sen", SEN_COST);
    if( gold ) me->pay_money(GOLD_COST);

    name = target->query("name");
    message_vision("$N手捏法訣，對著" + name + "唸動「" + cname + "」。\n", me);

    // 重骰：每一種武器技能各自重新抽（沒有保底，可能變好也可能變差）。
    foreach(sk in keys(target->query("damage"))) {
        if( undefinedp(limits[sk]) ) continue;
        target->set("damage/" + sk, reroll(limits[sk]));
    }
    main = main_skill(target);
    if( main && classp(dp = target->query("damage/" + main)) )
        tell_object(me, sprintf("%s的品質變了，傷害力 %d - %d（力量修正 %d%%，乘數等級 %d）。\n",
            name, dp->roll + dp->multipler, dp->roll + dp->multipler * dp->range,
            dp->bonus, dp->multipler));

    if( gold ) return 1;

    // 銀咒：非獨一無二的武器有機會損毀；經驗用標準悟性公式。
    if( userp(me) )
        me->improve_skill_exact(SKILL_ID, COMBAT_D->int_formula_exp(me));
    if( !function_exists("create_replica", target) && random(100) < break_chance(me) ) {
        if( target->query("equipped") ) target->unequip();
        tell_object(me, name + "承受不住法力，碎裂成片片廢鐵。\n");
        destruct(target);
    }
    return 1;
}
