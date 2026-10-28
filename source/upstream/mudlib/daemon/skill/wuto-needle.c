/*---
description: 武陀灸術【五敗針法】（wuto-needle）。武陀灸堂方士的攻擊針法，enable 在
             針灸之術（cauterization）上，以 acupuncture 施針。五針各削減目標一項屬性，
             並依雙方該項屬性傷害目標；施針即進入戰鬥。
design: docs/martial_arts/武陀灸堂_門派設計.md 第五章（2026-10-27 定案）
---*/
#include <ansi.h>

inherit SKILL;

#define EXP_TIER 5   // 經驗倍數 1～5（使用者定案，2026-10-28）
#include "/daemon/skill/wuto_needle.h"

#define SKILL_ID "wuto-needle"
#define KILL_CMD "/cmds/std/kill"

void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon(SKILL_ID);
    setup();
}

int valid_learn(object me) { return check_wuto(me); }

void skill_completed(object me, string skill)
{
    tell_object(me, HIY + gender_self(me) + "終於練成了五敗針法，五枚銀針在指間流轉，"
        "膽、骨、膂、韌、敏，針針都落在人身最軟的地方。\n" NOR);
}

/* W-01～W-05：屬性、傷害的精氣神、計算方式（avg 平均，sum 相加）。 */
mapping NEEDLES = ([
    "fear":   ([ "attr": "cor", "stats": ({ "gin", "kee", "sen" }), "calc": "avg",
                 "name": "敗膽一針", "color": HIR ]),
    "foul":   ([ "attr": "con", "stats": ({ "kee" }), "calc": "sum",
                 "name": "敗骨二針", "color": HIW ]),
    "fail":   ([ "attr": "str", "stats": ({ "kee" }), "calc": "sum",
                 "name": "敗膂三針", "color": HIM ]),
    "fling":  ([ "attr": "cps", "stats": ({ "gin", "sen" }), "calc": "sum",
                 "name": "敗韌四針", "color": HIY ]),
    "fumble": ([ "attr": "dex", "stats": ({ "gin", "sen" }), "calc": "sum",
                 "name": "敗敏五針", "color": HIG ]),
]);

// 下針敘述：$N 施針者、$n 目標、$W 手上的針、$T 招名（各針起手各不相同）。
mapping STRIKE_MSG = ([
    "fear":   "$N欺近$n身前，一招$T將$W扎進$n心口巨闕穴。",
    "foul":   "$N身形一側，反手以$W斜刺$n背後腎俞穴，正是$T。",
    "fail":   "$N左手虛按$n肩頭，右手$W順勢使出$T，扎進$n臂彎曲池穴。",
    "fling":  "$N搶到$n身後，$W點進$n腦後風池穴，這記$T快得無聲無息。",
    "fumble": "$N矮身一探，$W貼著$n腕底刺入內關穴，好一記$T。",
]);

// 中招敘述。
mapping HIT_MSG = ([
    "fear":   "$n心頭一陣發虛，冷汗涔涔而下，雙腿竟不由自主地打起顫來。",
    "foul":   "$n腰間一酸，渾身骨節像被抽去了筋，身子晃了兩晃。",
    "fail":   "$n兩臂一麻，手上氣力如潮水般退去，連兵刃都快握不住了。",
    "fling":  "$n腦中嗡的一響，心神渙散，眼前景物一陣搖晃。",
    "fumble": "$n指尖發麻，手腳頓時遲鈍了下來，一舉一動都慢了半拍。",
]);

private string needle_key(string spl)
{
    switch(spl) {
    case "fear": case "fear_first": case "敗膽一針": return "fear";
    case "foul": case "foul_second": case "敗骨二針": return "foul";
    case "fail": case "fail_third": case "敗膂三針": return "fail";
    case "fling": case "fling_fourth": case "敗韌四針": return "fling";
    case "fumble": case "fumble_fifth": case "敗敏五針": return "fumble";
    }
    return 0;
}

/* W-11：比照 kill 的限制。 */
private int can_attack(object me, object target)
{
    // 玩家不能對巫師下針；NPC（例如武道通）與巫師交手時照常施針。
    if( userp(me) && wizardp(target) ) return notify_fail("不行!!\n");
    if( environment(me)->query("no_fight") && !target->query("unprotect_mark") )
        return notify_fail("這裡不准戰鬥。\n");
    if( target->query("no_fight") ) return notify_fail("不行。\n");
    return 1;
}

int do_wuto(object me, string key, object target)
{
    mapping n = NEEDLES[key];
    string attr = n["attr"];
    int sk, mine, his, damage, amount, raw;

    sk = me->query_skill(SKILL_ID, 1);
    if( me->query_stat("sen") < 5 )
        return notify_fail(gender_self(me) + "的心神不足，捏不穩手中的針。\n");
    if( !can_attack(me, target) ) return 0;

    me->consume_stat("sen", 5);

    // W-13：以削減之前的雙方屬性計算傷害。
    mine = me->query_attr(attr);
    his = target->query_attr(attr);
    damage = n["calc"] == "avg" ? (mine + his) / 2 : mine + his;
    if( damage < 1 ) damage = 1;

    // 兩句：下針 → 中招；只有招名上色（高亮色換回青色前先 NOR）。
    needle_line(me, target, replace_string(replace_string(STRIKE_MSG[key],
        "$W", needle_name(me)), "$T", "「" + n["color"] + n["name"] + NOR CYN "」"));
    needle_blank(me);
    needle_line(me, target, HIT_MSG[key]);

    // W-11：可先出手、必中。已經在交手（fight 切磋或 kill 拼命）就維持原本的狀態。
    // 還沒開打時：NPC 強制進入切磋（fight），不會下殺手；玩家照 kill 的規定
    // （使用者定案，2026-10-28）。
    if( !me->is_fighting(target) ) {
        if( userp(target) )
            KILL_CMD->do_kill(me, target);
        else {
            me->fight_ob(target);
            target->fight_ob(me);
        }
    }

    // W-10：扣目前的值。
    foreach(string stat in n["stats"])
        target->consume_stat(stat, damage, me);

    // W-09：暫時削減屬性 1+sk/33，持續 sk/5 tick；同一針不疊加，重新計時。
    // 不把屬性削到 1 以下（以原本的屬性計，不含裝備）。
    amount = 1 + sk / 33;
    raw = target->query_attr(attr, 1);
    if( amount > raw - 1 ) amount = raw - 1;
    if( amount > 0 )
        set_needle_effect(target, "wuto_weaken_" + attr, key,
            ([ attr: -amount ]), sk / 5);

    // W-07：目標停頓 2 回合的機率 50% + sk/8%。
    if( random(100) < 50 + sk / 8 ) {
        needle_line(me, target, "$n身子一僵，一時竟提不起勁來。");
        target->start_busy(2);
    }
    COMBAT_D->report_status(target);

    needle_gain(me, SKILL_ID);
    me->start_busy(1);
    return 1;
}

varargs int acupuncture_cauterization(object me, string spl, object target)
{
    string key;
    object *enemies;

    if( !knows_needle(me, SKILL_ID) )
        return notify_fail(gender_self(me) + "還沒學會五敗針法。\n");
    if( !(key = needle_key(spl)) )
        return notify_fail("五敗針法沒有這種針法。\n");

    // 沒有指定對象時扎向正在交手的敵人。
    if( !objectp(target) ) {
        enemies = me->query_enemy();
        if( sizeof(enemies) ) target = enemies[0];
    }
    if( !objectp(target) || target == me || !objectp(target = needle_target(me, target)) )
        return notify_fail("這裡沒有可以下針的對象。\n");
    return do_wuto(me, key, target);
}
