/*---
description: 丹道【黃白之術】（alchemy-wealth）。龍圖丹派方士的法術類技能，
             安裝在 magic（enable magic with alchemy-wealth），以 conjure 施展
             銀咒（silver seal）、金咒（gold seal），把身上的物品轉成錢存入錢莊。
design: docs/martial_arts/龍圖丹派_黃白之術.md（2026-10-22）
note: 0 級即可施展；1～19 級不存在，累積到門檻後 gain 直接學成 20 級。
      80 級領悟金咒；之後只累積經驗，達到 100 級的累計值時直接從 80 升到 100；
      81～99 級不存在（2026-11-14 修改，原為 79 級卡住、90 級領悟金咒）。
      conjure 時可指定數量：on 20 <物品>、on all <物品>（2026-11-14）。法術修為、法術道行只拿系統規則，本檔不另外給。
      施法時同時累積基本法術（magic），失敗得 1/3（U-13，2026-10-24）。
---*/

#include <ansi.h>

inherit SKILL;

#define SKILL_ID     "alchemy-wealth"
#define SEN_COST     15
#define SILVER_LIMIT 2000    // 銀咒一次轉換的物品價值總和上限（W-03：含 2000，2026-11-14 修改）
#define GOLD_LEVEL   80      // 金咒領悟等級（W-01，2026-11-14 由 90 改為 80）
#define HOLD_LEVEL   80      // 80 級後只累積經驗（U-07，2026-11-14 由 79 改為 80）
#define JUMP_LEVEL   100     // 累積到 100 級的門檻時直接升到 100 級（U-07，2026-11-14）

void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon(SKILL_ID);
}

string type() { return "magic"; }

int valid_enable(string usage) { return usage == "magic"; }

// U-06：學成等級 20；門檻沿用系統預設（20^2 x 100 = 40,000）。
int query_entry_level() { return 20; }

// U-07：80 級時，經驗還沒到 100 級的累計值就不升級。
int query_skill_cap(object me)
{
    if( me->query_skill(SKILL_ID, 1) == HOLD_LEVEL
    &&  me->query_learn(SKILL_ID) < me->skill_threshold(SKILL_ID, JUMP_LEVEL) )
        return HOLD_LEVEL;
    return 0;
}

// U-07：80 級暫停升級時照常累積經驗，累積到 100 級的門檻才升級（#31，2026-11-04）。
// 沒有這個，技能系統會因為「已到上限」不再給經驗，80 級永遠卡住。
int query_learn_at_cap(object me)
{
    return me->query_skill(SKILL_ID, 1) == HOLD_LEVEL;
}

// U-06：學成訊息。
void skill_completed(object me, string sk)
{
    tell_object(me, gender_self(me) + "因為長期使用丹道【黃白之術】，所以"
        + gender_self(me) + "對這個法術已經逐漸熟悉。\n");
}

// U-07：升到 80 級時領悟金咒；從 80 升到 81 時直接跳到 100（2026-11-14）。
void skill_advanced(object me, string sk)
{
    int level = me->query_skill(sk, 1);

    if( level == GOLD_LEVEL ) {
        tell_object(me, HIY "由於" + gender_self(me) + "長期使用丹道【黃白之術】，使"
            + gender_self(me) + "對丹道【黃白之術】更加的熟悉，並領悟金咒。\n" NOR);
    }
    else if( level == HOLD_LEVEL + 1 ) {
        me->set_skill(sk, JUMP_LEVEL);
        tell_object(me, HIY + gender_self(me) + "對丹道【黃白之術】的體悟豁然貫通，"
            + "點金化銀的功夫更上一層樓。\n" NOR);
    }
}

// 2.6：換錢比例，以千分之一計（2026-11-14 修改）：
// 學成前 60%；學成（20 級）70%，之後每 5 級 +1%，到 140 級為 94%；
// 140 級後每級 +3.5%（200 級為 304%）。不設上限。
int wealth_rate(int level)
{
    int rate;

    if( level < 20 ) return 600;
    rate = 700 + ((level > 140 ? 140 : level) - 20) / 5 * 10;
    if( level > 140 ) rate += (level - 140) * 35;
    return rate;
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
// 經驗取「價值 ÷ 100（最多等級 × 100）」與「random(靈性＋悟性)」較大者（2026-11-07）。
void reward_success(object me, int value)
{
    int level, gain, base, n;

    level = me->query_skill(SKILL_ID, 1);
    gain = value / 100;
    if( level > 0 && gain > level * 100 ) gain = level * 100;
    n = me->query_attr("spi") + me->query_attr("int");
    base = n > 0 ? random(n) : 0;
    if( base > gain ) gain = base;
    if( gain > 0 ) me->improve_skill(SKILL_ID, gain);
    me->gain_score("alchemy", 1 + (level > 0 ? random(level) / 2 : 0));
    magic_gain(me, 0);
}

// U-04、U-10：失敗的經驗與丹道修養。經驗為 random(靈性＋悟性) ÷ 2（2026-11-07）。
void reward_failure(object me)
{
    int n, gain;

    n = me->query_attr("spi") + me->query_attr("int");
    gain = n > 0 ? random(n) / 2 : 0;
    if( gain > 0 ) me->improve_skill(SKILL_ID, gain);
    me->gain_score("alchemy", 1);
    magic_gain(me, 1);
}

// 一件物品的量詞（成疊物品用單位量詞，例如「根」松柴）。
string unit_of(object ob)
{
    string unit = ob->query("base_unit");

    if( !stringp(unit) ) unit = ob->query("unit");
    if( !stringp(unit) ) unit = "件";
    return unit;
}

int conjure_magic(object me, string spell, object target)
{
    string cname, light, unit, name;
    int level, value, total, money, want, have, count, stack;

    if( spell == "silver_seal" ) { cname = "銀咒"; light = "銀光"; }
    else if( spell == "gold_seal" ) { cname = "金咒"; light = "金光"; }
    else return notify_fail("丹道【黃白之術】沒有這種法術。\n");

    if( !objectp(target) )
        return notify_fail("指令格式﹕conjure " + replace_string(spell, "_", " ")
            + " on [數量|all] <物品>\n");

    level = me->query_skill(SKILL_ID, 1);

    // 2.3：不能轉的物品，直接拒絕（只有施法者看到）。
    if( forbidden(me, target) )
        return notify_fail(gender_self(me) + "所施用的" + cname + "，不可施用在這件物品上。\n");

    // W-25：尚未領悟金咒。
    if( spell == "gold_seal" && level < GOLD_LEVEL )
        return notify_fail(gender_self(me) + "尚未領悟金咒。\n");

    // W-11（2026-11-14）：數量由 conjure 指令傳來（conjure/amount）：
    // 沒指定為 1；-1 表示 all；不能疊的物品一律只算一件。
    stack = function_exists("query_amount", target) ? 1 : 0;
    have = stack ? target->query_amount() : 1;
    want = me->query_temp("conjure/amount");
    if( !stack ) count = 1;
    else if( want == -1 ) count = have;
    else if( want > 0 ) count = want;
    else count = 1;
    name = target->query("name");
    unit = unit_of(target);
    if( count > have )
        return notify_fail(gender_self(me) + "身上的" + name + "沒有那麼多" + unit + "。\n");

    // 2.4：銀咒一次轉換的價值總和不能超過 2000 文（含 2000，2026-11-14）；超過就整批拒絕。
    value = unit_value(target);
    total = value * count;
    if( spell == "silver_seal" && total > SILVER_LIMIT ) {
        if( count > 1 )
            return notify_fail(gender_self(me) + "所施展的銀咒，不足以一次轉化這麼多"
                + name + "。\n");
        return notify_fail(gender_self(me) + "所施展的銀咒，不足以轉化這" + unit
            + name + "。\n");
    }

    // W-21：神不足時不能施法，不扣神、不給經驗。
    if( me->query_stat("sen") < SEN_COST )
        return notify_fail(gender_self(me) + "的「神」不足以施展這個法術。\n");

    // W-20：固定消耗 15 神，失敗照扣；一次轉多個也只扣一次（2026-11-14）。
    me->consume_stat("sen", SEN_COST);
    message_vision("$N手捏法訣，對著" + name + "唸動「" + cname + "」。\n", me);

    // W-22、W-23：整批一起擲一次；失敗時物品全部留在身上。
    if( random(400) >= success_chance(me) ) {
        tell_object(me, gender_self(me) + "的法術失敗了。\n");
        reward_failure(me);
        me->set_temp("conjure/failed", 1);
        return 1;
    }

    // 2.6：存入錢莊；成功訊息不顯示存入金額（2026-11-14）。
    money = total * wealth_rate(level) / 1000;
    if( count > 1 )
        tell_object(me, chinese_number(count) + unit + name + "化作一道" + light
            + "，消失無蹤。\n");
    else
        tell_object(me, name + "化作一道" + light + "，消失無蹤。\n");
    me->add("bank_account", money);
    if( stack && have > count )
        target->add_amount(-count);
    else
        destruct(target);

    // U-03：經驗以整批物品價值總和算一次（2026-11-14）。
    reward_success(me, total);
    return 1;
}
