/*---
description: 天師道法【紫薇心經】（chivi sutra）。道士共通法術，enable 在法術（magic）上，以 conjure 施展：
             fire defence 禦火術、storm defence 禦風術、freeze defence 禦冰術、thunder defence 禦雷術，
             短時間提高對該系法術的防禦；dispel 強行逆運心經，消除身上的禦術。
             設計見 docs/martial_arts/天師道法_紫薇心經.md。
---*/
#include <ansi.h>

inherit SKILL;

string type() { return "magic"; }
int valid_enable(string usage) { return usage == "magic"; }

/* 回合計時：一個 tick 是兩秒。每次施展給一個編號，到期時編號對得上才解除（被 dispel 或重新施展過就不動）。 */
static int serial;

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("chivi sutra");
    setup();
}

/* 四系資料。顏色與天師道法施咒時相同：火紅、風紫、冰白、雷黃。 */
string kind_of(string spell)
{
    switch( spell ) {
    case "fire_defence":    return "fire";
    case "storm_defence":   return "storm";
    case "freeze_defence":  return "freeze";
    case "thunder_defence": return "thunder";
    }
    return 0;
}

string kind_color(string kind)
{
    if( kind == "fire" ) return HIR;
    if( kind == "storm" ) return HIM;
    if( kind == "freeze" ) return HIW;
    return HIY;
}

string kind_name(string kind)
{
    return ([ "fire": "禦火術", "storm": "禦風術", "freeze": "禦冰術", "thunder": "禦雷術" ])[kind];
}

/* 防禦力的裝備特性名稱，沿用戰鬥程式讀取的名稱（風 wind、冰 ice、雷 lightning）。 */
string armor_key(string kind)
{
    return ([ "fire": "armor_vs_fire", "storm": "armor_vs_wind",
              "freeze": "armor_vs_ice", "thunder": "armor_vs_lightning" ])[kind];
}

string cast_line(string kind)
{
    return ([
        "fire":    "$N默誦紫薇心經﹐周身泛起一圈赤紅光暈﹐祭起「禦火術」﹐四周灼人的熱氣漸漸退開。",
        "storm":   "$N默誦紫薇心經﹐周身泛起一圈紫色光暈﹐祭起「禦風術」﹐衣袂隨之垂落﹐週遭風勢到了身前便繞道而行。",
        "freeze":  "$N默誦紫薇心經﹐周身泛起一圈素白光暈﹐祭起「禦冰術」﹐侵人的寒意遇上光暈便如殘雪消融。",
        "thunder": "$N默誦紫薇心經﹐周身泛起一圈金黃光暈﹐祭起「禦雷術」﹐光暈之外隱隱有電光游走﹐卻近不得身。",
    ])[kind];
}

string fade_line(string kind)
{
    return ([
        "fire":    "你身上那圈赤紅光暈漸漸黯淡﹐禦火術的法力已盡。",
        "storm":   "你身上那圈紫色光暈漸漸黯淡﹐禦風術的法力已盡。",
        "freeze":  "你身上那圈素白光暈漸漸黯淡﹐禦冰術的法力已盡。",
        "thunder": "你身上那圈金黃光暈漸漸黯淡﹐禦雷術的法力已盡。",
    ])[kind];
}

/* 解除某一系禦術。id 對不上表示已經被消除或重新施展過，不處理。 */
varargs void remove_defence(object me, string kind, int id, int quiet)
{
    mapping info;

    if( !objectp(me) ) return;
    info = me->query_temp("chivi/" + kind);
    if( !mapp(info) || (id && info["id"] != id) ) return;
    me->add_temp("apply/" + armor_key(kind), -info["bonus"]);
    me->delete_temp("chivi/" + kind);
    if( !quiet )
        tell_object(me, kind_color(kind) + fade_line(kind) + "\n" NOR);
}

void expire(object me, string kind, int id)
{
    remove_defence(me, kind, id, 0);
}

int spend_sen(object me, int cost)
{
    if( me->query_stat("sen") < cost )
        return notify_fail("你的心神不足﹐無法施展紫薇心經。\n");
    me->consume_stat("sen", cost);
    return 1;
}

/* 禦X術：消耗神 10；效果 = 紫薇心經 + random(靈性)，持續 紫薇心經/3 個 tick（最少 1）。
 * 同一系的禦術生效中不能重複施展，四系可以同時存在。 */
int do_defence(object me, string kind)
{
    int sk, bonus, ticks, id;

    if( me->query_temp("chivi/" + kind) )
        return notify_fail("你身上已經有" + kind_name(kind) + "的法力了。\n");
    if( !spend_sen(me, 10) ) return 0;

    sk = me->query_skill("chivi sutra", 1);
    bonus = sk + random(me->query_attr("spi") + 1);
    ticks = sk / 3;
    if( ticks < 1 ) ticks = 1;
    id = ++serial;

    me->add_temp("apply/" + armor_key(kind), bonus);
    me->set_temp("chivi/" + kind, ([ "bonus": bonus, "id": id ]));
    message_vision(kind_color(kind) + cast_line(kind) + "\n" NOR, me, 0, 1);
    call_out("expire", ticks * 2, me, kind, id);
    return 1;
}

/* dispel：消耗神 5，把身上所有禦術的效果消除。 */
int do_dispel(object me)
{
    string *active = ({});

    foreach(string kind in ({ "fire", "storm", "freeze", "thunder" }))
        if( mapp(me->query_temp("chivi/" + kind)) ) active += ({ kind });
    if( !sizeof(active) )
        return notify_fail("你身上沒有紫薇心經的法力可以消除。\n");
    if( !spend_sen(me, 5) ) return 0;

    foreach(string kind in active) remove_defence(me, kind, 0, 1);
    message_vision(CYN "$N強行逆運紫薇心經﹐真氣反沖﹐週身的光暈應聲碎散。\n" NOR, me, 0, 1);
    return 1;
}

int conjure_magic(object me, string spell, object target)
{
    string kind;

    if( !me->query_skill("chivi sutra", 1) && !me->query_learn("chivi sutra") )
        return notify_fail("你還沒學會紫薇心經。\n");
    if( spell == "dispel" ) return do_dispel(me);
    if( kind = kind_of(spell) ) return do_defence(me, kind);
    return notify_fail("指令格式﹕conjure <法術名稱>\n");
}
