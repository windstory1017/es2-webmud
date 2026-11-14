/*---
description: 玄天劍訣（NEW 新版武功寫法）。玄天派劍法，可 enable 在劍術與拆招卸力之法；
             手動 perform 困天訣（hold）、破氣訣（kee-break）、凝精訣（gin-break）、隱神訣（sen-break）。
             設計見 docs/martial_arts/玄天教_門派設計.md。
---*/
#include <ansi.h>

inherit "/std/martial_art";

#define FACTION   "scholar.sky"
#define SKILL_ID  "advance-sky sword"
#define TICK_SECS 2          // 1 tick = 2 秒（同天邪掌的持續時間算法）
#define CD_TICKS  2          // 每個 perform 的冷卻（使用者定案，2026-11-14）
#define HIT_CAP   95         // 困天訣成功率、break 命中率的上限（%）

/* perform 資料：名稱、招式名稱顏色、break 打哪一項（使用者定案，2026-11-14）。
 * 整句白色（HIW），招式名稱用【】包住並套各自的顏色；成功、失敗、條件不足都同一規則。 */
private mapping PERFORM = ([
    "hold":      ([ "name": "困天訣", "color": HIG ]),
    "kee-break": ([ "name": "破氣訣", "color": HIR, "stat": "kee" ]),
    "gin-break": ([ "name": "凝精訣", "color": HIY, "stat": "gin" ]),
    "sen-break": ([ "name": "隱神訣", "color": HIB, "stat": "sen" ]),
]);

private void create()
{
    seteuid(getuid());

    art_id    = SKILL_ID;
    art_name  = "玄天劍訣";
    art_usage = "sword";
    art_desc  = "玄天派的劍法，劍意沉靜內斂，招式不求傷敵於一時，"
                + "更擅以劍意困人、奪人精氣神。";
    art_bonus = 0;

    moves = ({
        ([ "name": "雲開見日", "attack": 6, "defense": 5, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N足下一點﹐使出「雲開見日」﹐$w自下而上斜斜撩起﹐劍光一閃直取$n的$l" ]),
        ([ "name": "玄關暗渡", "attack": 7, "defense": 4, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N左手劍訣虛引﹐右手$w悄沒聲息遞出﹐一招「玄關暗渡」﹐劍尖已抵$n的$l" ]),
        ([ "name": "星垂平野", "attack": 6, "defense": 4, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N身形一矮﹐劍走低路﹐「星垂平野」﹐$w抖出數點寒星﹐點向$n的$l" ]),
        ([ "name": "天心月影", "attack": 6, "defense": 6, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N擰身迴劍﹐正是「天心月影」﹐$w劃出一彎淡月似的弧光﹐橫削$n的$l" ]),
        ([ "name": "兩儀轉乾", "attack": 5, "defense": 7, "force": 1, "bonus": 0, "damage_type": "割傷",
           "action": "$N劍訣一領﹐$w畫圓一轉﹐「兩儀轉乾」﹐劍勢忽陰忽陽﹐旋向$n的$l" ]),
    });

    // 練成 20 級（使用者定案，2026-11-14）；之後每級武術造詣 等級 × 10，
    // 武學之道 (等級 - 50) × 10，51 級起。
    art_entry     = 20;
    art_entry_msg = "你終於練成了玄天劍訣﹐劍意沉靜內斂﹐隱隱有雲氣在劍鋒之間流轉。\n";

    ma_coef  = 10;
    mm_start = 51;
    mm_minus = 50;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon(SKILL_ID);
    setup();
}

/* 劍術與拆招卸力之法（使用者定案，2026-11-14）。 */
int valid_enable(string usage)
{
    return usage == "sword" || usage == "parry";
}

/* 學習條件：玄天派弟子。 */
int valid_learn(object me)
{
    if( !is_faction(me, FACTION) )
        return notify_fail("你不是玄天派弟子﹐無法修練玄天劍訣。\n");
    return 1;
}

/* 招架：拆招卸力敘述（$n 是招架者，$N 是攻擊者）。手上沒有劍時用系統的格擋敘述。 */
string parry_message(object me, object attacker, int full)
{
    object w;
    string wn;

    w = me->query_temp("weapon/sword");
    if( !objectp(w) ) return 0;
    wn = w->name();
    return ({
        "$n橫劍一圈﹐貼著$N的兵刃順勢一帶﹐將來勢化於無形",
        "$n左手劍訣輕輕一按﹐右手" + wn + "微微回收﹐借勢卸去$N這一招的力道",
        "$n身形不動﹐" + wn + "在身前劃出一道圓弧﹐把$N的攻勢輕輕引向一旁",
    })[random(3)];
}

/* 招架加成：內功同時 enable 玄天心法時，格擋力道上限 ×2（使用者定案，2026-11-14）。
 * 這是提供給戰鬥程式（feature/char/combat.c 的 parry_attack）詢問的倍率；
 * 戰鬥程式要加一行呼叫才會生效，那是系統面修改，尚待使用者同意。 */
int parry_multiplier(object me)
{
    return me->skill_mapped("force") == "sky force" ? 2 : 1;
}

/* 困天訣成功率（%）：sk ÷ 4 + 悟性，最高 95（使用者定案，2026-11-14）。 */
int hold_chance(object me)
{
    int c;

    c = me->query_skill(SKILL_ID, 1) / 4 + me->query_attr("int");
    return c > HIT_CAP ? HIT_CAP : c;
}

/* break 命中率（%）：困天訣成功率的 2 倍，最高 95。 */
int break_chance(object me)
{
    int c;

    c = hold_chance(me) * 2;
    return c > HIT_CAP ? HIT_CAP : c;
}

/* break 傷害：(悟性 + sk ÷ 5) × 2/3 ～ (悟性 + sk ÷ 5)。 */
int break_damage(object me)
{
    int top, low;

    top = me->query_attr("int") + me->query_skill(SKILL_ID, 1) / 5;
    if( top < 1 ) top = 1;
    low = top * 2 / 3;
    return low + random(top - low + 1);
}

/* 整句白色，招式名稱【】用各自的顏色。 */
private string say_it(string act, string before, string after)
{
    mapping p = PERFORM[act];

    return HIW + before + p["color"] + "【" + p["name"] + "】" + HIW + after + NOR + "\n";
}

private string fail_msg(string act, string before, string after)
{
    return say_it(act, before, after);
}

/* 在交手的對手裡挑目標；沒指定就取第一個。 */
private object pick_target(object me, object target)
{
    object ob, *enemies;

    enemies = filter(me->query_enemy(), (: objectp($1) :));
    if( !sizeof(enemies) ) return 0;
    if( !objectp(target) ) return enemies[0];
    foreach(ob in enemies)
        if( ob == target ) return target;
    return 0;
}

private int hold_action(object me, object victim)
{
    int cost;

    cost = me->query_stat_maximum("gin") / 10;
    if( me->query_stat("gin") <= cost )
        return notify_fail(fail_msg("hold", "你的精力不足﹐施展不出", "。"));

    me->consume_stat("gin", cost);
    me->start_busy(1);
    if( random(100) < hold_chance(me) ) {
        message_vision(say_it("hold", "$N左手劍訣一指﹐使出",
            "﹐一道沉靜的劍意籠住$n﹐$n只覺周身氣機一滯"), me, victim);
        victim->start_busy(2);
    }
    else
        message_vision(say_it("hold", "$N劍訣一指﹐",
            "的劍意罩向$n﹐$n卻心神一凝﹐掙開了去"), me, victim);
    return 1;
}

private int break_action(object me, object victim, string act)
{
    int cost, dmg;
    string stat, wn;
    object w;

    stat = PERFORM[act]["stat"];
    cost = me->query_stat_maximum("sen") / 8;
    if( me->query_stat("sen") < cost + 1 )
        return notify_fail(fail_msg(act, "你的心神不足﹐施展不出", "。"));

    me->consume_stat("sen", cost);
    w = me->query_temp("weapon/sword");
    wn = objectp(w) ? w->name() : "長劍";    // 武器名稱本身帶顏色，後面要接回白色（HIW）

    if( random(100) >= break_chance(me) ) {
        switch(act) {
        case "kee-break":
            message_vision(say_it(act, "$N使出", "﹐劍氣點向$n的氣脈﹐卻被$n察覺偏了開去"), me, victim);
            break;
        case "gin-break":
            message_vision(say_it(act, "$N使出", "﹐劍意探向$n的靈台﹐$n卻凝神守住﹐劍意無功而返"), me, victim);
            break;
        default:
            message_vision(say_it(act, "$N使出", "﹐劍意無聲無息探向$n的心神﹐卻被$n一口氣斷了去路"), me, victim);
        }
        return 1;
    }

    switch(act) {
    case "kee-break":
        message_vision(say_it(act, "$N" + wn + HIW + "輕輕一點﹐使出", "﹐一縷劍氣逕取$n的氣脈﹐$n胸口一悶"), me, victim);
        break;
    case "gin-break":
        message_vision(say_it(act, "$N" + wn + HIW + "微顫﹐使出", "﹐劍意直透$n的靈台﹐$n眼前一花﹐精神為之一渙"), me, victim);
        break;
    default:
        message_vision(say_it(act, "$N劍意隱於無形﹐使出", "﹐悄悄侵入$n的心神﹐$n神思一恍"), me, victim);
    }

    /* 傷害對手對應狀態的當前值；傷害的 1/4 恢復自身對應狀態的有效上限（格子），
     * 傷害的 1/2 恢復自身對應狀態的現在值。 */
    dmg = break_damage(me);
    victim->consume_stat(stat, dmg, me);
    me->heal_stat(stat, dmg / 4);
    me->supplement_stat(stat, dmg / 2);
    message_vision(HIW "$N只覺一股精氣隨著劍意回流﹐丹田之中沉靜了許多。\n" NOR, me);
    return 1;
}

/* perform sword:hold、kee-break、gin-break、sen-break（使用者定案，2026-11-14）。
 * 每個 perform 各自冷卻 2 tick；困天訣自身停頓 1 tick，三個 break 自身不停頓。 */
int perform_action(object me, string act, object target)
{
    object victim;

    if( undefinedp(PERFORM[act]) )
        return notify_fail("玄天劍訣沒有這種絕招。\n");

    victim = pick_target(me, target);
    if( !objectp(victim) )
        return notify_fail("你並未與這個人交手。\n");

    if( me->query_temp("sky_sword/cd/" + act) > time() )
        return notify_fail(fail_msg(act, "你的劍意尚未平復﹐還施展不出", "。"));

    if( act == "hold" ) {
        if( !hold_action(me, victim) ) return 0;
    }
    else if( !break_action(me, victim, act) ) return 0;

    me->set_temp("sky_sword/cd/" + act, time() + CD_TICKS * TICK_SECS);
    return 1;
}
