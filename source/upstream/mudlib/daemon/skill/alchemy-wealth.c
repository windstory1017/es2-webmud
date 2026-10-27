/*---
description: 丹道【黃白之術】（alchemy-wealth）。龍圖丹派方士的法術類技能，
             安裝在 magic（enable magic with alchemy-wealth），以 conjure 施展
             銀咒（silver seal）、金咒（gold seal），把身上的物品轉成錢存入錢莊。
design: docs/martial_arts/龍圖丹派_黃白之術.md（2026-10-22）
note: 0 級即可施展；1～19 級不存在，累積到門檻後 gain 直接學成 20 級。
      79 級後只累積經驗，達到 90 級的累計值時直接從 79 升到 90 並領悟金咒；
      80～89 級不存在。法術修為、法術道行只拿系統規則，本檔不另外給。
      施法時同時累積基本法術（magic），失敗得 1/3（U-13，2026-10-24）。
---*/

#include <ansi.h>

inherit SKILL;

#define SKILL_ID     "alchemy-wealth"
#define SEN_COST     15
#define SILVER_LIMIT 500     // 銀咒只能轉價值 500 文以下（W-03：未滿 500）
#define GOLD_LEVEL   90      // 金咒領悟等級（W-01）
#define HOLD_LEVEL   79      // 79 級後只累積經驗（U-07）

void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon(SKILL_ID);
}

string type() { return "magic"; }

int valid_enable(string usage) { return usage == "magic"; }

// U-06：學成等級 20；門檻沿用系統預設（20^2 x 100 = 40,000）。
int query_entry_level() { return 20; }

// U-07：79 級時，經驗還沒到 90 級的累計值就不升級。
int query_skill_cap(object me)
{
    if( me->query_skill(SKILL_ID, 1) == HOLD_LEVEL
    &&  me->query_learn(SKILL_ID) < me->skill_threshold(SKILL_ID, GOLD_LEVEL) )
        return HOLD_LEVEL;
    return 0;
}

// U-06：學成訊息。
void skill_completed(object me, string sk)
{
    tell_object(me, gender_self(me) + "因為長期使用丹道【黃白之術】，所以"
        + gender_self(me) + "對這個法術已經逐漸熟悉。\n");
}

// U-07：從 79 升到 80 時直接跳到 90，領悟金咒。
void skill_advanced(object me, string sk)
{
    if( me->query_skill(sk, 1) == HOLD_LEVEL + 1 ) {
        me->set_skill(sk, GOLD_LEVEL);
        tell_object(me, HIY "由於" + gender_self(me) + "長期使用丹道【黃白之術】，使"
            + gender_self(me) + "對丹道【黃白之術】更加的熟悉，並領悟金咒。\n" NOR);
    }
}

// 把以文為單位的金額寫成錢莊寫法（同 cmds/std/balance.c）。
string money_string(int amount)
{
    string str = "";

    if( amount >= 10000 ) str += chinese_number(amount / 10000) + "兩黃金";
    if( amount % 10000 >= 100 ) str += chinese_number(amount % 10000 / 100) + "兩碎銀";
    if( amount % 100 ) str += chinese_number(amount % 100) + "文錢";
    return str == "" ? "零文錢" : str;
}

// 2.6：換錢比例（百分比）。
int wealth_rate(int level)
{
    if( level < 20 )  return 65;
    if( level < 50 )  return 70;
    if( level < 90 )  return 75;
    if( level < 120 ) return 80;
    if( level < 150 ) return 85;
    if( level < 180 ) return 90;
    return 95;
}

// W-22：成功率＝等級÷4＋50%，最高 95%（以 1/400 計，避免整數除法誤差）。
int success_chance(object me)
{
    int chance;

    chance = me->query_skill(SKILL_ID, 1) + 200;
    if( chance > 380 ) chance = 380;
    return chance;
}

// 一個（成疊物品只算一個）的價值。
int unit_value(object ob)
{
    if( function_exists("query_amount", ob) ) return ob->query("base_value");
    return ob->query("value");
}

// 2.3：不能轉的物品。
int forbidden(object me, object ob)
{
    if( environment(ob) != me ) return 1;                       // g 地上的東西
    if( ob->query("money_id") ) return 1;                       // a 錢
    if( unit_value(ob) <= 0 ) return 1;                         // b 價值 0
    if( ob->query("equipped") ) return 1;                       // c 穿戴中
    if( ob->query_max_encumbrance() > 0
    ||  ob->query("liquid_container") ) return 1;               // d 容器
    if( living(ob) || ob->is_character() || ob->is_corpse() ) return 1; // e
    if( ob->query("no_sell") || ob->query("no_drop") ) return 1; // f
    return 0;
}

// U-13（2026-10-24，使用者指示 #12）：施法時同時累積基本法術（magic），照茅山法術的基本法術公式，
// 層級固定 1：（random(靈性)＋1）×（random(靈性)＋1）＋random(靈性)。
// 失敗時取 1/3（無條件捨去），最少 1。
void magic_gain(object me, int failed)
{
    int spi, gain;

    if( !userp(me) ) return;
    spi = me->query_attr("spi");
    if( spi < 1 ) spi = 1;
    gain = (random(spi) + 1) * (random(spi) + 1) + random(spi);
    if( failed ) {
        gain /= 3;
        if( gain < 1 ) gain = 1;
    }
    me->improve_skill_exact("magic", gain);
}

// U-03、U-10：成功的經驗與丹道修養。
void reward_success(object me, int value)
{
    int level, gain;

    level = me->query_skill(SKILL_ID, 1);
    gain = value / 100;
    if( level > 0 && gain > level * 100 ) gain = level * 100;
    if( gain > 0 ) me->improve_skill(SKILL_ID, gain);
    me->gain_score("alchemy", 1 + (level > 0 ? random(level) / 2 : 0));
    magic_gain(me, 0);
}

// U-04、U-10：失敗的經驗與丹道修養。
void reward_failure(object me)
{
    int n, gain;

    n = me->query_attr("spi") + me->query_attr("int");
    gain = n > 0 ? random(n) : 0;
    if( gain > 0 ) me->improve_skill(SKILL_ID, gain);
    me->gain_score("alchemy", 1);
    magic_gain(me, 1);
}

int conjure_magic(object me, string spell, object target)
{
    string cname, light, unit;
    int level, value, money;

    if( spell == "silver_seal" ) { cname = "銀咒"; light = "銀光"; }
    else if( spell == "gold_seal" ) { cname = "金咒"; light = "金光"; }
    else return notify_fail("丹道【黃白之術】沒有這種法術。\n");

    if( !objectp(target) )
        return notify_fail("指令格式﹕conjure " + replace_string(spell, "_", " ")
            + " on <物品>\n");

    level = me->query_skill(SKILL_ID, 1);

    // 2.3：不能轉的物品，直接拒絕（只有施法者看到）。
    if( forbidden(me, target) )
        return notify_fail(gender_self(me) + "所施用的" + cname + "，不可施用在這件物品上。\n");

    // W-25：尚未領悟金咒。
    if( spell == "gold_seal" && level < GOLD_LEVEL )
        return notify_fail(gender_self(me) + "尚未領悟金咒。\n");

    // 2.4：銀咒轉不動 500 文以上的物品。
    value = unit_value(target);
    if( spell == "silver_seal" && value >= SILVER_LIMIT ) {
        unit = target->query("base_unit");
        if( !stringp(unit) ) unit = target->query("unit");
        if( !stringp(unit) ) unit = "件";
        return notify_fail(gender_self(me) + "所施展的銀咒，不足以轉化這" + unit
            + target->query("name") + "。\n");
    }

    // W-21：神不足時不能施法，不扣神、不給經驗。
    if( me->query_stat("sen") < SEN_COST )
        return notify_fail(gender_self(me) + "的「神」不足以施展這個法術。\n");

    // W-20：固定消耗 15 神，失敗照扣。
    me->consume_stat("sen", SEN_COST);
    message_vision("$N手捏法訣，對著" + target->query("name") + "唸動「" + cname + "」。\n", me);

    // W-22、W-23：失敗時物品留在身上。
    if( random(400) >= success_chance(me) ) {
        tell_object(me, gender_self(me) + "的法術失敗了。\n");
        reward_failure(me);
        me->set_temp("conjure/failed", 1);
        return 1;
    }

    // 2.6：存入錢莊，成疊的只轉一個（W-11）。
    money = value * wealth_rate(level) / 100;
    tell_object(me, target->query("name") + "化作一道" + light + "，消失無蹤。"
        + gender_self(me) + "的錢莊存款增加了" + money_string(money) + "。\n");
    me->add("bank_account", money);
    if( function_exists("query_amount", target) && target->query_amount() > 1 )
        target->add_amount(-1);
    else
        destruct(target);

    reward_success(me, value);
    return 1;
}
