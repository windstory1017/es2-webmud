/*---
description: 穹槍訣（NEW 新版武功寫法）。共通槍法，可用單手槍、雙手槍；特色是滿天槍點與蓄力一擊，
             文字取星空、蒼穹的意象。設計見 docs/martial_arts/穹槍訣.md。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

/* 招名：亮綠色『』，之後接回一般顏色。 */
#define MOVE(x) HIG "『" x "』" NOR

private void create()
{
    seteuid(getuid());

    art_id    = "sky_pike";
    art_name  = "穹槍訣";
    art_usage = "pike";
    art_desc  = "一套以星空蒼穹為意的槍法，槍尖抖處如滿天星點，"
                + "蓄勢而發時又有破穹之力；單手、雙手槍皆可施展。";
    art_bonus = 0;

    /* 前四招滿天槍點：命中高、傷害平；後兩招蓄力一擊：命中低、傷害重。 */
    moves = ({
        ([ "name": "星垂平野", "attack": 8, "defense": 3, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N槍尖斜指地面﹐驀地一抖﹐使出" MOVE("星垂平野") "﹐槍頭化作點點寒星﹐自下而上罩向$n的$l" ]),
        ([ "name": "天河倒掛", "attack": 9, "defense": 2, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N雙手一送﹐槍桿翻起一道銀弧﹐正是" MOVE("天河倒掛") "﹐一串槍花接連不斷地扎向$n的$l" ]),
        ([ "name": "斗轉星移", "attack": 7, "defense": 5, "force": 1, "bonus": 0, "damage_type": "刺傷",
           "action": "$N腳踏星位斜走三步﹐$w隨身一擰﹐一招" MOVE("斗轉星移") "﹐槍尖由左而右連點$n的$l" ]),
        ([ "name": "七星連珠", "attack": 9, "defense": 2, "force": 2, "bonus": 5, "damage_type": "刺傷",
           "action": "$N手腕連抖七下﹐槍尖在空中點出北斗之形﹐" MOVE("七星連珠") "一點接一點直指$n的$l" ]),
        ([ "name": "穹頂壓城", "attack": 5, "defense": 3, "force": 3, "bonus": 15, "damage_type": "刺傷",
           "action": "$N沉腰坐馬﹐將全身勁力凝於槍尖﹐槍未發已隱隱有風雷之聲﹐" MOVE("穹頂壓城") "猛地扎向$n的$l" ]),
        ([ "name": "破曉穿雲", "attack": 6, "defense": 2, "force": 3, "bonus": 20, "damage_type": "刺傷",
           "action": "$N蓄勢良久﹐忽地一聲長嘯﹐使出" MOVE("破曉穿雲") "﹐$w挾著破空銳響﹐一往無前地貫向$n的$l" ]),
    });

    /* 學成：累積到 20 級門檻時 gain，直接練成 20 級；學成那一級起每級給獎勵。 */
    art_entry        = 20;
    art_entry_reward = 1;
    art_entry_msg    = "你終於練成了穹槍訣﹐槍尖一抖﹐眼前彷彿也綻開了點點星光。\n";

    /* 每級武術造詣 等級 × 10；41 級起武學之道 (等級 − 40) × 10。 */
    ma_coef  = 10;
    mm_start = 41;
    mm_minus = 40;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("sky_pike");
    setup();
}

/* 單手槍、雙手槍；不能招架。 */
int valid_enable(string usage) { return usage == "pike" || usage == "twohanded pike"; }

/* 100 級、140 級各加 1 點膂力（各只給一次）。 */
void skill_advanced(object me, string skill)
{
    int lv;

    ::skill_advanced(me, skill);
    if( !userp(me) ) return;
    lv = me->query_skill("sky_pike", 1);
    foreach(int step in ({ 100, 140 })) {
        if( lv < step || me->query("sky_pike/str_" + step) ) continue;
        me->set("sky_pike/str_" + step, 1);
        me->set_attr("str", me->query_attr("str", 1) + 1);
        tell_object(me, HIY "你的膂力提高了！\n" NOR);
    }
}

int two_handed(object weapon)
{
    return objectp(weapon) && weapon->query("equipped") == "weapon/twohanded pike";
}

/* 單手槍特攻『萬點寒星』：對手停頓 1 tick。 */
void star_rain(object me, object opponent)
{
    message_vision(HIW "$N一槍落空﹐卻不收勢﹐手腕順勢急抖﹐槍尖霎時化出" MOVE("萬點寒星") HIW "﹐\n"
        "恍如滿天星斗一齊傾落﹐虛虛實實﹐將$n周身要害盡數罩住﹗\n"
        "$n只覺眼前星光亂閃﹐不知該擋哪一點﹐登時手忙腳亂﹐身形為之一滯。\n" NOR, me, opponent, 1);
    opponent->start_busy(1);
}

/* 雙手槍特攻『破穹一擊』：該招傷害 200%，不能閃躲、招架；自身停頓 1 tick。 */
void sky_breaker(object me, object opponent, object weapon)
{
    message_vision(HIW "$N一槍落空﹐竟借著回勢沉肩擰腰﹐將渾身勁力盡數灌入槍身﹗\n" NOR, me, opponent, 1);
    special_hit(me, opponent, weapon,
        ([ "name": "破穹一擊", "attack": 6, "defense": 2, "force": 3, "bonus": 20, "damage_type": "刺傷",
           "action": "$N雙手握槍一聲暴喝﹐" MOVE("破穹一擊") "直如天柱傾倒﹐挾著撕裂長空的銳嘯﹐迎面貫向$n的$l" ]),
        200, 0, HIT_MUST);
    if( objectp(me) ) {
        message_vision("$N這一擊用盡了力氣﹐一時收不回槍勢。\n", me);
        me->start_busy(1);
    }
}

/* 100 級起，一般出招沒命中（沒造成傷害）時判定：單手槍 20%、雙手槍 10%。 */
void after_strike(object me, object opponent, object weapon, mapping m, int damage)
{
    if( damage > 0 || !objectp(opponent) || !living(me) ) return;
    if( environment(me) != environment(opponent) ) return;
    if( me->query_skill("sky_pike", 1) < 100 ) return;
    if( two_handed(weapon) ) {
        if( random(100) < 10 ) sky_breaker(me, opponent, weapon);
    } else if( random(100) < 20 ) star_rain(me, opponent);
}
