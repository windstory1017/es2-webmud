/*---
description: 傲劍訣（NEW 新版武功寫法）。古劍門專屬劍法，可 enable 在劍術、雙手劍與招架（拆招卸力）。
             強度比現有最強劍招（冷梅劍法、六陰追魂劍法）再強 2 成。
             特攻「劍領風騷」（120 級起）連發數道劍氣，各扣固定的氣；招架成功（60 級起）回復氣。
             招式與格擋敘述取自舊站景愈濤的戰鬥畫面。
             設計見 docs/martial_arts/傲劍訣_景愈濤戰鬥敘述.md。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

#define SWORD_SPECIAL_LEVEL  120   // 劍領風騷
#define PARRY_RECOVER_LEVEL   60   // 招架成功回復氣

private void create()
{
    seteuid(getuid());

    art_id    = "au-chan sword";
    art_name  = "傲劍訣";
    art_usage = "sword";
    art_desc  = "古劍門的鎮派劍訣，劍意瀟灑華麗、大開大闔；招式一氣呵成，"
                + "劍舞「劍領風騷」更能連發數道劍氣，招架時亦能借力調息。";
    art_bonus = 20;

    moves = ({
        ([ "name": "皓風起兮雲飛揚", "attack": 9,  "defense": 4, "force": 2, "bonus": 0,  "damage_type": "刺傷",
           "action": "$N使出劍訣起手式「皓風起兮雲飛揚」﹐手中$w如一條銀蛇般攻向$n的$l" ]),
        ([ "name": "騰躍萬里風如煙", "attack": 9,  "defense": 5, "force": 2, "bonus": 5,  "damage_type": "割傷",
           "action": "$N一招「騰躍萬里風如煙」﹐身形陡然幻化成虛影﹐手中$w大開大闔﹐火速斬向$n的$l" ]),
        ([ "name": "千嬌百媚柔似水", "attack": 10, "defense": 5, "force": 1, "bonus": 0,  "damage_type": "刺傷",
           "action": "$N劍光一閃﹐一招「千嬌百媚柔似水」﹐挑起$w﹐輕靈曼妙的刺向$n的$l" ]),
        ([ "name": "清風送爽塵雪飛", "attack": 10, "defense": 4, "force": 2, "bonus": 5,  "damage_type": "割傷",
           "action": "$N一招「清風送爽塵雪飛」﹐手中$w劍氣大盛﹐$w挾著勁風迎面攻$n的$l" ]),
        ([ "name": "了絕紅塵遁山林", "attack": 9,  "defense": 3, "force": 3, "bonus": 5,  "damage_type": "割傷",
           "action": "$N深深一嘆﹐一招「了絕紅塵遁山林」使將開來﹐手中$w真氣貫注﹐劍芒四射﹐一道長虹貫過$n的$l" ]),
        ([ "name": "佳人在擁醉臥君", "attack": 9,  "defense": 6, "force": 1, "bonus": 0,  "damage_type": "刺傷",
           "action": "$N劍轉反側﹐一招「佳人在擁醉臥君」﹐手中$w上刺﹐隨後迴鋒下引攻$n的$l" ]),
        ([ "name": "持劍立地為生靈", "attack": 9,  "defense": 4, "force": 2, "bonus": 5,  "damage_type": "刺傷",
           "action": "$N使出劍訣中的「持劍立地為生靈」劍光點點﹐隱藏著無數後著﹐刺向$n的$l" ]),
        ([ "name": "英雄江山美人絕", "attack": 10, "defense": 3, "force": 3, "bonus": 10, "damage_type": "割傷", "weight": 3,
           "action": "$N笑仰蒼天﹐只見劍光閃閃﹐氣漩四起﹐$N挑起手中$w﹐一招「英雄江山美人絕」﹐挾萬鈞之勢﹐無數劍氣將$n透體而過" ]),
    });

    art_entry     = 20;
    art_entry_msg = "你終於練成了傲劍訣。\n";
    level_msgs    = ([ 120: "你領悟了傲劍訣精義【劍領風騷】！\n" ]);

    ma_coef  = 10;
    mm_start = 30;
    mm_minus = 29;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("au-chan sword");
    setup();
}

/* 可當劍術、雙手劍與招架（拆招卸力）使用。 */
int valid_enable(string usage)
{
    return usage == "sword" || usage == "twohanded sword" || usage == "parry";
}

/* 學習條件：古劍門專屬。 */
int valid_learn(object me)
{
    if( !is_faction(me, "fighter.au-chan") )
        return notify_fail("你不是古劍門弟子﹐無法修練傲劍訣。\n");
    return 1;
}

object sword_in_hand(object me)
{
    object w;

    if( objectp(w = me->query_temp("weapon/twohanded sword")) ) return w;
    return me->query_temp("weapon/sword");
}

int can_follow(object me, object opponent)
{
    return objectp(me) && objectp(opponent) && living(me)
        && environment(me) == environment(opponent);
}

/* 劍領風騷：每一道劍氣的字、顏色、固定扣氣。 */
#define KIND_NAME   0
#define KIND_COLOR  1
#define KIND_WORD   2
#define KIND_KEE    3

mixed *beam_kind(int r)
{
    // r：0～99；狂 5%，其餘五式各 19%。
    if( r < 5 ) return ({ "狂", HIY, "金", 50 });
    switch( (r - 5) / 19 ) {
        case 0:  return ({ "禮", BLU, "藍", 15 });
        case 1:  return ({ "德", CYN, "青", 18 });
        case 2:  return ({ "忠", GRN, "綠", 20 });
        case 3:  return ({ "義", MAG, "紫", 22 });
        default: return ({ "武", HIR, "紅", 25 });
    }
}

/* 劍領風騷：傲劍訣 120 級起，出招時機率 (等級 ÷ 10)%（整數）；
 * 先決定出幾招＝1 ＋ random(定力 ÷ 7)，再各自決定是哪一式。
 * 劍氣不能閃躲、格擋，也不被防具減免，直接扣固定的氣。 */
void do_sword_dance(object me, object opponent, object weapon)
{
    int i, n;
    mixed *kind;

    n = 1 + random(me->query_attr("cps", 1) / 7);
    message_vision(HIW "\n$N猛然施展傲劍訣中華麗的劍舞「" HIR "劍領" HIC "風騷" HIW "」！\n\n" NOR, me, opponent, 1);
    for( i = 0; i < n; i++ ) {
        if( !can_follow(me, opponent) ) break;
        kind = beam_kind(random(100));
        message_vision(HIW "$N施展傲劍訣中的極招「" HIR "劍領" HIC "風騷" HIW "」" + kind[KIND_COLOR]
            + "『" + kind[KIND_NAME] + "』" HIW "﹐一道" + kind[KIND_COLOR] + kind[KIND_WORD]
            + "色" HIW "的劍氣射向$n！\n" NOR, me, opponent, 1);
        opponent->receive_damage(kind[KIND_KEE], me, me);
        if( objectp(opponent) ) show_status(opponent, kee_ratio(opponent));
    }
}

void attack_using(object me, object opponent, object weapon)
{
    int lv;

    if( !opponent ) return;
    if( !objectp(weapon) ) weapon = 0;

    lv = me->query_skill("au-chan sword", 1);
    if( lv >= SWORD_SPECIAL_LEVEL && objectp(weapon) && random(100) < lv / 10 ) {
        do_sword_dance(me, opponent, weapon);
        return;
    }
    ::attack_using(me, opponent, weapon);
}

/* 招架：拆招卸力敘述（$n 是招架者，$N 是攻擊者）。完全擋下與部分格擋各自挑選。 */
string parry_message(object me, object attacker, int full)
{
    object w;
    string wn;

    w = sword_in_hand(me);
    wn = objectp(w) ? w->name() : "雙手";
    if( full )
        return ({
            "$n手中" + wn + "劍芒大盛﹐一招「緲盡人世絕惡意」將$N的攻勢完全封住",
            "$n身體有如陀螺急轉﹐一招「不識江山多嬌媚」將$N的所有攻勢帶開",
            "$n將手中" + wn + "一展﹐一式「天始一乾景初開」將$N的攻勢化開",
        })[random(3)];
    return ({
        "$n將手中" + wn + "一橫﹐一招「橫劍破」﹐擋住了$N的攻勢",
        "$n將手中" + wn + "一揮﹐使出一式「輕柔小扇撲流螢」將$N的攻勢化解掉",
        "$n將手中" + wn + "一展﹐一式「天始一乾景初開」將$N的攻勢化開",
    })[random(3)];
}

void recover_kee(object me)
{
    int n;

    if( !objectp(me) || !living(me) ) return;
    n = me->query_attr("con", 1) + me->query_skill("au-chan sword", 1) / 10;
    me->supplement_stat("kee", n);
}

/* 招架成功（含完全擋下）：傲劍訣 60 級起，回復氣＝根骨 ＋ 傲劍訣等級 ÷ 10。 */
void parry_success(object me, object attacker, int full)
{
    if( me->query_skill("au-chan sword", 1) < PARRY_RECOVER_LEVEL ) return;
    // 等對方這一擊的訊息顯示完再回復。
    call_out("recover_kee", 0, me);
}
