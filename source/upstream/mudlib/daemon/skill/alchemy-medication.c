/*---
description: 丹道【神農之術】（alchemy-medication）。龍圖丹派方士的法術類技能，
             安裝在 magic（enable magic with alchemy-medication），以 conjure 施展
             七色印，封住或解開青銅丹鼎。
design: docs/martial_arts/龍圖丹派_神農之術.md（2026-10-15；2026-10-22 修改 U-10、U-11）
note: 0 級即可施展；0～14 級不存在，累積到學成門檻後 gain 直接學成 15 級。
      升級不增加精、氣、神；法術修為、法術道行只拿系統規則（2026-10-22）。
---*/

#include <ansi.h>

inherit SKILL;

#define SKILL_ID   "alchemy-medication"
#define SEN_COST   15
#define MAX_CHANCE 95

// 印的顏色：英文 -> 中文（S-01，原版七色）
private mapping colors = ([
    "green":  "青",
    "white":  "白",
    "red":    "赤",
    "purple": "紫",
    "yellow": "黃",
    "gray":   "灰",
    "black":  "皂",
]);

void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon(SKILL_ID);
}

string type() { return "magic"; }

int valid_enable(string usage) { return usage == "magic"; }

// U-06：學成等級 15；門檻沿用系統預設（15^2 x 100 = 22,500）。
int query_entry_level() { return 15; }

// U-08：學成訊息。
void skill_completed(object me, string sk)
{
    tell_object(me, gender_self(me) + "因為長期使用丹道【神農之術】，所以"
        + gender_self(me) + "對這個法術已經逐漸熟悉。\n");
}

// U-11（2026-10-22）：取消 31 級起的法術修為；升級不增加精、氣、神。

// S-32：成功率（靈性＋悟性×2＋技能等級＋10）%，最高 95%。
int success_chance(object me)
{
    int chance;

    chance = me->query_attr("spi") + me->query_attr("int") * 2
        + me->query_skill(SKILL_ID, 1) + 10;
    if( chance > MAX_CHANCE ) chance = MAX_CHANCE;
    return chance;
}

// U-03、U-10：成功的經驗與丹道修養。
void reward_success(object me)
{
    int level, spi, intel;

    spi = me->query_attr("spi");
    intel = me->query_attr("int");
    level = me->query_skill(SKILL_ID, 1);
    me->improve_skill(SKILL_ID,
        (spi + (intel * 5 > 0 ? random(intel * 5) : 0)) * 2);
    // U-10（2026-10-22）：1 + random(等級) ÷ 2，無條件捨去。
    me->gain_score("alchemy", 1 + (level > 0 ? random(level) / 2 : 0));
}

// U-04、U-10：失敗的經驗與丹道修養。
void reward_failure(object me)
{
    int n, gain;

    n = me->query_attr("spi") + me->query_attr("int");
    gain = n > 0 ? random(n) : 0;
    if( gain > 0 ) me->improve_skill(SKILL_ID, gain);
    me->gain_score("alchemy", 1);
}

int conjure_magic(object me, string spell, object target)
{
    string color, action, cname, seal_name, current;
    int ok;

    if( sscanf(spell, "%s_%s", color, action) != 2
    ||  undefinedp(colors[color])
    ||  (action != "seal" && action != "dispel") )
        return notify_fail("丹道【神農之術】沒有這種法術。\n");

    if( !objectp(target) || !target->is_heat_reactor() )
        return notify_fail("這個法術只能對青銅丹鼎施展。\n");

    // S-31：神不足時不能施法，不扣神、不給經驗。
    if( me->query_stat("sen") < SEN_COST )
        return notify_fail(gender_self(me) + "的「神」不足以施展這個法術。\n");

    cname = colors[color];
    seal_name = (action == "seal" ? "" : "散") + cname + "印";

    // S-30：固定消耗 15 神，失敗照扣。
    me->consume_stat("sen", SEN_COST);
    message_vision("$N用手在" + target->name() + "上虛畫了一個「" + seal_name
        + "」，口中念念有詞地祝禱著。\n", me);

    // S-33：重複封印、錯色解印、解沒有封印的丹鼎，一律算失敗。
    current = target->query_seal();
    if( action == "seal" ) ok = !stringp(current);
    else ok = stringp(current) && current == color;

    // S-32：成功率。
    if( ok && random(100) >= success_chance(me) ) ok = 0;

    if( !ok ) {
        tell_object(me, gender_self(me) + "的法術失敗了。\n");
        reward_failure(me);
        me->set_temp("conjure/failed", 1);
        return 1;
    }

    message_vision(target->name() + "泛起一陣光芒，然後漸漸褪去 ....\n", me);
    if( action == "seal" ) {
        target->set_seal(color);
        tell_object(me, gender_self(me) + "的" + seal_name + "效力已經成功的被"
            + target->name() + "吸收了！\n");
    } else {
        target->set_seal(0);
        tell_object(me, gender_self(me) + "的" + seal_name + "效力已經成功地解開"
            + target->name() + "上的封印了！\n");
    }
    reward_success(me);
    return 1;
}
