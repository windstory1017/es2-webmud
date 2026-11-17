#include <ansi.h>
inherit SKILL;

/* 殘蟻六象步法（哭笑門閃躲，無特功）。閃避敘述：第一句取自「劉乙忘玄 vs 韓笑」對戰紀錄，
 * 其餘自撰。只有真的閃過時才出現，接在對方出招敘述後面（$N 出招的人、$n 閃避的人）。 */
string *dodge_actions = ({
    "$n腳踩六象步﹐一個踉蹌﹐似拙實巧地閃過$N",
    "$n身子一歪﹐腳下六象步亂中有序﹐橫跨三步已繞到$N身側",
    "$n雙足交錯踏出象步﹐肩頭一晃﹐竟從$N的招式縫隙中滑了出去",
    "$n裝作立足不穩向後一仰﹐腳尖在地上輕輕一點﹐借力退出數尺",
    "$n踩著六象步連轉幾圈﹐似醉似狂﹐$N的攻勢全落在空處",
    "$n怪笑一聲﹐腳下象步一踏一蹬﹐貼著$N的招式溜了開去",
});

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("chin-steps");
    setup();
}

int valid_enable(string usage)
{
    return usage == "dodge";
}

/* 學習條件：哭笑門弟子。 */
int valid_learn(object me)
{
    if( me->query("custom_faction") != "fighter.huge" )
        return notify_fail("你不是哭笑門弟子﹐殘蟻六象步法的步子你學不來。\n");
    return 1;
}

// 學成：照一般門檻累積到 10 級時 gain，直接練成 10 級。
int query_entry_level() { return 10; }

void skill_completed(object me, string sk)
{
    tell_object(me, HIY "你終於練成了殘蟻六象步法。\n" NOR);
}

/* 每級獎勵（超過學成等級才給）：武術造詣 等級 × 10；31 級起武學之道 (等級 - 30) × 10。 */
void skill_advanced(object me, string sk)
{
    int lv;

    if( !userp(me) ) return;
    lv = me->query_skill("chin-steps", 1);
    if( lv <= 10 ) return;
    me->gain_score("martial art", lv * 10);
    if( lv > 30 ) me->gain_score("martial mastery", (lv - 30) * 10);
}

int dodge_using(object me, int ability, int strength, object from)
{
    object attacker;

    if( objectp(me) && objectp(from) ) {
        attacker = from;
        if( !from->is_character()
        &&  objectp(environment(from)) && environment(from)->is_character() )
            attacker = environment(from);

        if( attacker->is_character() && attacker != me )
            attacker->set_temp("defend_message",
                dodge_actions[random(sizeof(dodge_actions))]);
    }

    return me->query_skill("dodge");
}
