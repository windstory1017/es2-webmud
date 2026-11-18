/*---
description: 玄冰烈焰斧（NEW 新版武功寫法）。雙手斧武功，六招火向、冰向各三招，
             附帶火焰、冰寒傷害；100 級起命中後機率追擊絕招「玄冰烈焰」，
             190 級起附加的屬性傷害加倍。設計見 docs/martial_arts/玄冰烈焰斧.md。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

private void create()
{
    seteuid(getuid());

    art_id    = "ice-fire axe";
    art_name  = "玄冰烈焰斧";
    art_usage = "twohanded axe";
    art_desc  = "程家世代相傳的重斧功夫，大開大闔之間隱含烈焰與玄冰之力，"
                + "斧越沉重勁道越猛，使到極處冰火交迫，威力驚人。";
    art_bonus = 33;   // 整套傷害 133%

    /* 火向三招（招名紅字）、冰向三招（招名青字）。招式名稱顏色只套在招名的字上。 */
    moves = ({
        ([ "attack": 6, "defense": 2, "force": 2, "bonus": 0, "damage_type": "砍傷", "element": "fire",
           "action": "$N雙臂一振﹐$w掠過處空氣隱隱扭曲發燙﹐一招「" RED "赤炎開山" NOR "」當頭劈向$n的$l" ]),
        ([ "attack": 6, "defense": 3, "force": 1, "bonus": -5, "damage_type": "割傷", "element": "fire",
           "action": "$N擰腰轉身把$w掄成一道赤弧﹐「" RED "熔金斷流" NOR "」帶著灼人的熱浪橫掃$n的$l" ]),
        ([ "attack": 6, "defense": 2, "force": 2, "bonus": 5, "damage_type": "砍傷", "element": "fire",
           "action": "$N大步搶進﹐這招「" RED "焚野燎原" NOR "」斧斧連環﹐$w過處熱風撲面地剁向$n的$l" ]),
        ([ "attack": 6, "defense": 3, "force": 1, "bonus": 0, "damage_type": "割傷", "element": "ice",
           "action": "$N沉腰坐馬﹐$w自下而上帶起絲絲寒氣﹐「" CYN "寒潭鎖月" NOR "」斜斜撩向$n的$l" ]),
        ([ "attack": 5, "defense": 2, "force": 2, "bonus": 10, "damage_type": "瘀傷", "element": "ice",
           "action": "$N縱身躍起把$w高舉過頂﹐這一記「" CYN "冰河崩岸" NOR "」斧鋒凝著薄霜﹐重重砸向$n的$l" ]),
        ([ "attack": 6, "defense": 3, "force": 1, "bonus": -10, "damage_type": "割傷", "element": "ice",
           "action": "$N倒踏半步蓄足了勁﹐一招「" CYN "霜刃摧岳" NOR "」$w橫斬而出﹐寒意先一步逼到$n的$l" ]),
    });

    /* 學成：累積到 20 級門檻時 gain，直接練成 20 級；學成那一級起每級給武術造詣 等級 × 10。 */
    art_entry        = 20;
    art_entry_reward = 1;
    art_entry_msg    = "你終於練成了玄冰烈焰斧﹐斧起處一邊熱浪翻湧﹐一邊寒氣逼人。\n";

    /* 每級武術造詣 等級 × 10；41 級起武學之道 (等級 - 40) × 15；不設升級訊息。 */
    ma_coef  = 10;
    mm_start = 41;
    mm_minus = 40;
    mm_coef  = 15;

    DAEMON_D->register_skill_daemon("ice-fire axe");
    setup();
}

/* 只能 enable 在雙手斧術。 */
int valid_enable(string usage) { return usage == "twohanded axe"; }

/* 學習條件：僅武者；種族、性別不限。任務進度由程槐檢查。 */
int valid_learn(object me)
{
    if( me->query_class() != "fighter" )
        return notify_fail("這路斧法講的是武者的根基﹐你的底子不對﹐練不來。\n");
    return 1;
}

/* 武器越重傷害越高：每公斤 +1%，最高 +30%。 */
private int weight_pct(object weapon)
{
    int kg;

    if( !objectp(weapon) ) return 100;
    kg = weapon->query_weight() / 1000;
    if( kg > 30 ) kg = 30;
    if( kg < 0 ) kg = 0;
    return 100 + kg;
}

private int can_follow(object me, object opponent)
{
    return objectp(me) && objectp(opponent) && living(me) && environment(me) == environment(opponent);
}

/* 一般招式：傷害再乘上重量加成。 */
void attack_using(object me, object opponent, object weapon)
{
    mapping m;
    int damage;

    if( !opponent ) return;
    if( !objectp(weapon) ) weapon = 0;
    m = pick_move(me);
    damage = COMBAT_D->fight(me, opponent, art_id, make_action(m, weight_pct(weapon)), weapon);
    after_strike(me, opponent, weapon, m, damage);
}

/* 絕招「玄冰烈焰」：追擊一斧，傷害 150%，同時帶火焰與冰寒傷害；追擊本身不再觸發。 */
private void chase(object me, object opponent, object weapon, mapping m, int lv)
{
    mapping act;
    int result, extra;

    me->set_temp("ice_fire_axe/chasing", 1);
    message_vision("\n" HIY "$N長嘯一聲將" + weapon->name() + "掄過頭頂﹐絕招「玄冰烈焰」"
        "挾開天闢地之勢轟然劈向$n﹗\n\n" NOR, me, opponent, 1);

    act = copy(m);
    act["element"] = "fire";   // 先帶火焰（fight() 自己會加一次），冰寒另外補
    result = special_hit(me, opponent, weapon, act, 150 * weight_pct(weapon) / 100, 100, HIT_SILENT);
    if( result > 0 && objectp(opponent) ) {
        extra = lv >= 190 ? 2 : 1;                       // 190 級起附加屬性傷害加倍
        while( extra-- > 0 && objectp(opponent) )
            COMBAT_D->elemental_extra(me, opponent, "ice");
        if( lv >= 190 && objectp(opponent) )
            COMBAT_D->elemental_extra(me, opponent, "fire");
    }
    if( objectp(opponent) ) show_status(opponent, kee_ratio(opponent));
    me->delete_temp("ice_fire_axe/chasing");
}

/* 出招之後：190 級起火向／冰向招式的附加傷害再算一次；100 級起機率追擊。 */
void after_strike(object me, object opponent, object weapon, mapping m, int damage)
{
    int lv, chance;

    if( damage <= 0 || !can_follow(me, opponent) ) return;
    if( me->query_temp("ice_fire_axe/chasing") ) return;
    lv = me->query_skill(art_id, 1);

    if( lv >= 190 && stringp(m["element"]) )
        COMBAT_D->elemental_extra(me, opponent, m["element"]);
    if( !can_follow(me, opponent) ) return;

    if( lv < 100 || !objectp(weapon) ) return;
    chance = lv / 10;
    if( chance > 20 ) chance = 20;
    if( random(100) < chance ) chase(me, opponent, weapon, m, lv);
}
