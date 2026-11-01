/*---
description: 武陀灸術【引源針法】（potency-needle）。武陀灸堂方士的輔助針法，enable 在
             針灸之術（cauterization）上，以 acupuncture 施針：fatigue 緩解疲勞；
             awareness、move、element 強化警覺、行動力或元素傷害（三種只能有一種）。
             學習條件：經脈穴道之學 80。
design: docs/martial_arts/武陀灸堂_門派設計.md 第六章（2026-10-27 定案）
---*/
#include <ansi.h>

inherit SKILL;

#define EXP_TIER 3   // 經驗倍數 1～3（使用者定案，2026-10-28）
#include "/daemon/skill/wuto_needle.h"

#define SKILL_ID "potency-needle"
#define CND      "wuto_potency"

void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon(SKILL_ID);
    setup();
}

int valid_learn(object me)
{
    if( !check_wuto(me) ) return 0;
    if( me->query_skill("kee anatomy", 1) < 80 )
        return notify_fail(gender_self(me) + "對經脈穴道的認識還太淺，摸不著引源針法的門徑。\n");
    return 1;
}

void skill_completed(object me, string skill)
{
    tell_object(me, HIY + gender_self(me) + "終於練成了引源針法，循經取穴之間，"
        "已能引動人身潛藏的本源之力。\n" NOR);
}

/* P-02～P-04：主項 sk/2，另外兩項 sk/5；元素傷害四種元素都加（P-07）。 */
private mapping potency_apply(string kind, int sk)
{
    mapping apply = ([]);
    int aware, move, element;

    aware = kind == "awareness" ? sk / 2 : sk / 5;
    move = kind == "move" ? sk / 2 : sk / 5;
    element = kind == "element" ? sk / 2 : sk / 5;
    apply["awarness"] = aware;   // 系統的警覺欄位就叫 awarness
    apply["move"] = move;
    foreach(string e in ({ "fire", "ice", "lightning", "wind" }))
        apply["damage_vs_" + e] = element;
    return apply;
}

private string kind_name(string kind)
{
    return ([ "awareness": "耳目", "move": "腳力", "element": "元氣" ])[kind];
}

// 下針與效果敘述：$W 換成手上的針；「$n」在扎自己時，下針句換成「自己」、效果句換成「$N」。
mapping ACTION = ([
    "fatigue":   ({ "$N拈起$W，在$n足三里、湧泉兩穴各捻入一針。",
                    "$n長長吁了口氣，臉上的倦色淡了幾分。" }),
    "awareness": ({ "$N捻起$W，刺入$n耳後翳風穴。",
                    "$n頓覺耳聰目明，四周一草一木的動靜都清清楚楚。" }),
    "move":      ({ "$N持著$W，沿著$n小腿接連扎下三針。",
                    "$n只覺一股暖流直通足底，腳步登時輕快了許多。" }),
    "element":   ({ "$N將$W一針扎進$n掌心勞宮穴。",
                    "$n掌中隱隱發燙，體內元氣躁動，似乎隨時要破掌而出。" }),
]);

/* 比照天師道法的格式：下針 → 效果，整句青色；引源針法不取招式名，沒有起手句。 */
void show_needle(object me, object target, string kind)
{
    string act = replace_string(ACTION[kind][0], "$W", needle_name(me)), res = ACTION[kind][1];

    if( target == me ) {
        act = replace_string(act, "$n", "自己");
        res = replace_string(res, "$n", "$N");
    }
    needle_line(me, target, act);
    needle_blank(me);
    needle_line(me, target, res);
}

/* P-01：消耗神 20，緩解疲勞 5+sk/40。 */
int do_fatigue(object me, object target, int sk)
{
    int tired, n;

    if( me->query_stat("sen") < 20 )
        return notify_fail(gender_self(me) + "的心神不足，捏不穩手中的針。\n");
    tired = target->query_stat("fatigue");
    if( tired <= 0 )
        return notify_fail((target == me ? gender_self(me) : target->name())
            + "精神飽滿，並沒有什麼疲累可解。\n");

    me->consume_stat("sen", 20);
    n = 5 + sk / 40;
    if( n > tired ) n = tired;
    target->consume_stat("fatigue", n, me);
    show_needle(me, target, "fatigue");
    return 1;
}

int do_potency(object me, string kind, object target, int sk)
{
    mapping data;

    if( me->query_stat("sen") < 10 )
        return notify_fail(gender_self(me) + "的心神不足，捏不穩手中的針。\n");

    // P-05：三種強化同一目標只能有一種；同一種再扎一次重新計時。
    data = target->query_condition(CND);
    if( mapp(data) && data["kind"] != kind )
        return notify_fail((target == me ? gender_self(me) : target->name())
            + "體內被引動的" + kind_name(data["kind"]) + "之力未散，再引另一股只怕經脈承受不住。\n");

    me->consume_stat("sen", 10);
    show_needle(me, target, kind);
    // P-06、P-08：持續 sk 個 tick，對自己不加倍。
    set_needle_effect(target, CND, kind, potency_apply(kind, sk), sk);
    return 1;
}

varargs int acupuncture_cauterization(object me, string spl, object target)
{
    int sk, ok;

    if( !knows_needle(me, SKILL_ID) )
        return notify_fail(gender_self(me) + "還沒學會引源針法。\n");
    if( !objectp(target = needle_target(me, target)) )
        return notify_fail("這裡沒有可以施針的對象。\n");

    sk = me->query_skill(SKILL_ID, 1);
    switch(spl) {
    case "fatigue":
        ok = do_fatigue(me, target, sk);
        break;
    case "awareness": case "move": case "element":
        ok = do_potency(me, spl, target, sk);
        break;
    default:
        return notify_fail("引源針法沒有這種針法。\n");
    }
    if( !ok ) return 0;
    needle_gain(me, SKILL_ID);
    me->start_busy(1);
    return 1;
}
