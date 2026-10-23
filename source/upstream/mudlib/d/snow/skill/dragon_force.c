/*---
description: 龍圖心經（dragon force）。龍圖丹派的內功，安裝在 force
             （enable force with dragon force）。exert dragon force 打坐修練；
             練成後可用錢莊存款轉化丹藥（exert lavage／relax／refresh／mana／heal／
             minor heal／major heal）。
design: docs/martial_arts/龍圖丹派_龍圖心經.md（2026-10-22）
note: 1～69 級不存在，累積到門檻後 gain 直接練成 70 級。門檻不打折。
---*/

#include <ansi.h>

inherit SKILL;

#define SKILL_ID    "dragon force"
#define ENTRY_LEVEL 70
#define MAJOR_LEVEL 160      // 160 級起 exert major heal 轉化大還丹

// 第四章：可轉化的丹藥。exert 功能名 -> ({ 丹藥檔, 神農之術門檻, 消耗的狀態 })
// 消耗：gin／kee／sen 扣目前的值 5；fatigue 疲勞 +3；空字串不扣。
private mapping pills = ([
    "lavage":     ({ "/obj/alchemist/lavage_pill",     5,   ""        }),
    "relax":      ({ "/obj/alchemist/relax_pill",      20,  "fatigue" }),
    "refresh":    ({ "/obj/alchemist/refresh_pill",    50,  "gin"     }),
    "mana":       ({ "/obj/alchemist/mana_pill",       55,  "sen"     }),
    "minor heal": ({ "/obj/alchemist/minor_heal_pill", 60,  "kee"     }),
    "major heal": ({ "/obj/alchemist/major_heal_pill", 120, "kee"     }),
]);

void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon(SKILL_ID);
}

int valid_enable(string usage)
{
    return usage == "force";
}

// 練成等級 70；門檻沿用系統預設（取消舊版的打一折）。
int query_entry_level() { return ENTRY_LEVEL; }

// 練成：慧根 +1（只給一次）。
void skill_completed(object me, string sk)
{
    tell_object(me,
        HIY + gender_self(me) + "覺得腹中一股暖洋洋的熱氣忽然膨脹，立刻充滿了全身各處，看來"
        + gender_self(me) + "的龍圖心經已經練成了！\n" NOR);
    if( !me->query("dragon_force/bonus_entry") ) {
        me->set("dragon_force/bonus_entry", 1);
        me->set_attr("wis", me->query_attr("wis", 1) + 1);
        tell_object(me, HIY + gender_self(me) + "的慧根提高了。\n" NOR);
    }
}

// 第三章：升級獎勵。
void skill_advanced(object me, string sk)
{
    int level;

    level = me->query_skill(sk, 1);
    if( level <= ENTRY_LEVEL ) return;      // 練成那一級只給慧根（skill_completed）

    // 每升一級神 +1、氣 +1；120 級起再加氣 +4、精 +5。
    me->advance_stat("sen", 1);
    me->advance_stat("kee", 1);
    if( level >= 120 ) {
        me->advance_stat("kee", 4);
        me->advance_stat("gin", 5);
    }

    // 100 級：膂力 +2、慧根 +1（只給一次）。
    if( level >= 100 && !me->query("dragon_force/bonus_100") ) {
        me->set("dragon_force/bonus_100", 1);
        me->set_attr("str", me->query_attr("str", 1) + 2);
        me->set_attr("wis", me->query_attr("wis", 1) + 1);
        tell_object(me, HIY + gender_self(me) + "的龍圖心經修為大進，周身真氣運轉自如。\n"
            + gender_self(me) + "的膂力提高了。\n"
            + gender_self(me) + "的慧根提高了。\n" NOR);
    }

    // U-02：內功基礎分數公式，71 級起。
    me->gain_score("martial art", level * 10);
    me->gain_score("martial mastery", (level - 29) * 10);
}

int do_exercise(object me)
{
    if( me->query_stat("gin") < 10
    ||    me->query_stat("kee") < 10
    ||    me->query_stat("sen") < 10 ) {
	tell_object(me, "你覺得神困力乏﹐沒有辦法繼續練功了。\n");
	return 0;   // 傳回 0 結束打坐（系統沒有 interrupt_me，舊寫法會一直重複這句）
    }
    me->consume_stat("gin", 10);
    me->consume_stat("kee", 10);
    me->consume_stat("sen", 10);

    if( random(me->query_attr("cps") + me->query_attr("con") + me->query_skill("force", 1)) > 20 ) {
	me->damage_stat("gin", 1);
	me->damage_stat("kee", 1);
	me->damage_stat("sen", 1);
	/* Preserve Dragon Force's canonical special-skill gain, but route the
	 * accompanying BASIC force learned through the user-restored formula. */
	// 龍形神功點數（使用者定案，2026-10-19）：悟性公式 + random(靈性)。
	me->improve_skill_exact("dragon force", COMBAT_D->int_formula_exp(me)
	    + random(me->query_attr("spi")));
	me->improve_restored_force_tick();
    }
    return 1;
}

int halt_exercise(object me, object owner, object from, string how)
{
    return 1;
}

// 4.2：花費倍數（十分之一）。
int price_rate(int level)
{
    if( level <= 100 ) return 90;
    if( level <= 120 ) return 80;
    if( level <= 140 ) return 70;
    if( level <= 160 ) return 60;
    if( level <= 180 ) return 55;
    return 50;
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

// 第四章：以錢莊存款轉化丹藥。
int make_pill(object me, string func)
{
    mixed *info;
    object pill;
    string stat, pname;
    int level, price, fatigue;

    level = me->query_skill(SKILL_ID, 1);
    if( level < ENTRY_LEVEL )
        return notify_fail(gender_self(me) + "的龍圖心經尚未練成，無法轉化丹藥。\n");

    // G-03、G-04：heal 的分流。
    if( func == "heal" ) {
        if( level >= MAJOR_LEVEL )
            return notify_fail(gender_self(me) + "已能以龍圖心經轉化小還丹與大還丹，\n"
                "請指定要轉換的丹藥：exert minor heal 或 exert major heal。\n");
        func = "minor heal";
    }
    if( func == "major heal" && level < MAJOR_LEVEL )
        return notify_fail(gender_self(me) + "的龍圖心經修為不足，無法轉化大還丹。\n");

    info = pills[func];
    pname = load_object(info[0])->query("name");

    // G-02：只看神農之術原生等級。
    if( me->query_skill("alchemy-medication", 1) < info[1] )
        return notify_fail(gender_self(me) + "對" + pname + "的藥性所知不足，無法轉化。\n");

    stat = info[2];
    if( (stat == "gin" || stat == "kee" || stat == "sen") && me->query_stat(stat) < 5 )
        return notify_fail(gender_self(me) + "的" + to_chinese(stat)
            + "不足，無法運起龍圖心經。\n");

    price = load_object(info[0])->query("base_value") * price_rate(level) / 10;
    if( me->query("bank_account") < price )
        return notify_fail(gender_self(me) + "在錢莊的存款不足以轉化" + pname + "。\n");

    pill = new(info[0]);
    if( !pill->move(me) ) {
        destruct(pill);
        return notify_fail(gender_self(me) + "身上已經放不下" + pname + "了。\n");
    }

    me->add("bank_account", -price);
    if( stat == "fatigue" ) {
        fatigue = me->query_stat("fatigue") + 3;
        if( me->query_stat_maximum("fatigue") > 0
        &&  fatigue > me->query_stat_maximum("fatigue") )
            fatigue = me->query_stat_maximum("fatigue");
        me->set_stat_current("fatigue", fatigue);
    } else if( stat != "" )
        me->consume_stat(stat, 5);

    message_vision("$N運起龍圖心經，雙掌間氤氳流轉，凝成一粒" + pname + "。\n", me);
    tell_object(me, gender_self(me) + "的錢莊存款減少了" + money_string(price) + "。\n");
    me->start_busy(1);
    return 1;
}

varargs int
exert_function(object me, string func, object target)
{
    switch(func) {
    case "dragon force":
	message_vision(HIY "$N盤膝而坐，深深地吸了口氣，開始修習龍圖心經的內功。\n" NOR,
	    me);
	me->start_busy((: do_exercise, me :), (: halt_exercise, me :));
	return 1;
    case "lavage":
    case "relax":
    case "refresh":
    case "mana":
    case "heal":
    case "minor heal":
    case "major heal":
	return make_pill(me, func);
    default:
	return notify_fail("龍圖心經沒有這種功能。\n");
    }
}
