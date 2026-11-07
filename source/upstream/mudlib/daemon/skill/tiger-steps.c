#include <ansi.h>
inherit SKILL;

/* 閃避敘述（取自「劉乙忘玄 vs 韓笑」對戰紀錄）：接在對方出招敘述後面，
 * 只有閃過時才出現。$N 是出招的人，$n 是閃避的人。 */
string *dodge_actions = ({
    "$n就地一個溜滾﹐一個『餓虎逐狼步』﹐飛快橫身裡撲﹐打亂$N的攻勢",
    "$n一個『虎奔步』﹐轉眼間竟繞過$N﹐向前疾奔出數丈",
    "$n一聲悶吼﹐身子暴然彈起﹐從$N頭頂掠了過去",
    "$n不閃不退﹐身形倏然翻滾﹐一招『虎躍風生』令$N撲了個空",
    "$n往右一個急跨﹐『猛虎爬山』步法順勢而發﹐瞬間閃過$N的攻擊",
});

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("tiger-steps");
    setup();
}

int valid_enable(string usage)
{
    return usage == "dodge";
}

// 學成：照一般門檻累積到 10 級時 gain，直接練成 10 級。
int query_entry_level() { return 10; }

void skill_completed(object me, string sk)
{
    tell_object(me, HIY "你已經掌握了狻猊步法。\n" NOR);
}

/* 每級獎勵（超過學成等級才給）：武術造詣 等級 × 10；
 * 41 級起武學之道 (等級 - 40) × 10。 */
void skill_advanced(object me, string sk)
{
    int lv;

    if( !userp(me) ) return;
    lv = me->query_skill("tiger-steps", 1);
    if( lv <= 10 ) return;
    me->gain_score("martial art", lv * 10);
    if( lv > 40 ) me->gain_score("martial mastery", (lv - 40) * 10);
}

/*
 * 閃避機率照 ES2 原本的公式，步法只提供有效閃避值。
 * 這裡先選好一句閃避敘述交給出招的一方；戰鬥程式只有在真的閃過時
 * 才會把這句接到出招敘述後面，沒閃過就不會出現。
 */
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
