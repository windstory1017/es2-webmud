/*---
description: 青邪杖法（NEW 新版武功寫法）。哭笑門的看家杖法，可用單手杖、雙手杖，也可 enable 在招架上。
             招式與招架敘述取自「劉乙忘玄 vs 韓笑」對戰紀錄，設計見 docs/martial_arts/哭笑門_青邪杖法.md。
---*/
#include <ansi.h>
#include "/std/martial_art.h"

inherit "/std/martial_art";

private void create()
{
    seteuid(getuid());

    art_id    = "chin_staff";
    art_name  = "青邪杖法";
    art_usage = "twohanded staff";
    art_desc  = "哭笑門的看家杖法，攻守兼備而偏重防守；招式看似瘋癲無章，"
                + "杖頭所向卻處處暗藏殺機。單手杖、雙手杖皆可施展，也能用來招架。";
    art_bonus = 0;

    /* 招名「傲視青天腰」中間一字影片被遮住，暫定「視」，待使用者確認（見設計文件）。
     * lvl 是解鎖等級：學成時就是 20 級，所以前四招一開始就會，殺招要練到 50 級。 */
    moves = ({
        ([ "name": "雲散獨不笑", "lvl": 0, "attack": 7, "defense": 5, "force": 2, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N手中$w一轉﹐杖頭突然一招『雲散獨不笑』直直朝$n$l撞去" ]),
        ([ "name": "擺頭嘲月老", "lvl": 0, "attack": 7, "defense": 6, "force": 2, "bonus": 0, "damage_type": "瘀傷",
           "action": "$N一杖橫掃﹐正是一招『擺頭嘲月老』重重掄向$n$l" ]),
        ([ "name": "一怒摧山倒", "lvl": 0, "attack": 6, "defense": 2, "force": 4, "bonus": 10, "damage_type": "瘀傷",
           "action": "$N提起蠻力﹐一招『一怒摧山倒』朝$n$l狠狠擊了下去" ]),
        ([ "name": "傲視青天腰", "lvl": 20, "attack": 8, "defense": 4, "force": 3, "bonus": 5, "damage_type": "瘀傷",
           "action": "$N將$w一頂﹐單手橫甩﹐一招『傲視青天腰』$w甩向$n$l" ]),
        ([ "name": "亂杖碎天山", "lvl": 50, "attack": 9, "defense": 3, "force": 4, "bonus": 15, "damage_type": "瘀傷",
           "weight": 3,
           "action": "$N一聲清嘯﹐殺招『亂杖碎天山』﹐十二招錯綜杖法連續擊向$n$l" ]),
    });

    art_entry     = 20;
    art_entry_msg = "你終於練成了青邪杖法﹐橫杖一掃﹐勁風過處隱隱已有撼山之勢。\n";

    ma_coef  = 10;
    mm_start = 30;
    mm_minus = 29;
    mm_coef  = 10;

    DAEMON_D->register_skill_daemon("chin_staff");
    setup();
}

/* 單手杖、雙手杖、招架。 */
int valid_enable(string usage)
{
    return usage == "staff" || usage == "twohanded staff" || usage == "parry";
}

/* 學習條件：哭笑門弟子。 */
int valid_learn(object me)
{
    if( !is_faction(me, "fighter.huge") )
        return notify_fail("你不是哭笑門弟子﹐青邪杖法的路數你學不來。\n");
    return 1;
}

/* 招架：拆招敘述（$n 是招架者，$N 是攻擊者）。沒有拿杖時用系統預設敘述。 */
string parry_message(object me, object attacker, int full)
{
    object w;
    string wn;

    w = me->query_temp("weapon/twohanded staff");
    if( !objectp(w) ) w = me->query_temp("weapon/staff");
    if( !objectp(w) ) return 0;
    wn = w->name();
    return ({
        "$n口中喃喃有聲﹐將手中的" + wn + "舞得密不透風﹐狠狠震開$N的兵刃",
        "$n連聲怪叫﹐勢若瘋癲﹐揮舞著" + wn + "擊開$N的兵刃",
        "$n眼露兇光﹐手中" + wn + "狂砸亂掃﹐竟打散了$N的攻勢",
        "$n一聲虎吼﹐手中" + wn + "猛擊而下﹐撞開$N手中兵刃",
        "$n在生死存亡之際﹐陡然暴起一股蠻力﹐將$N的兵刃打偏",
    })[random(5)];
}
