/*---
description: 天師道法【紫薇心經】（chivi sutra）。道士與方士共通法術，enable 在法術（magic）上，以 conjure 施展：
             fire defence 禦火術、storm defence 禦風術、freeze defence 禦冰術、thunder defence 禦雷術，
             短時間提高對該系法術的防禦；dispel 強行逆運心經，消除身上的禦術。
             設計見 docs/martial_arts/天師道法_紫薇心經.md。
---*/
#include <ansi.h>

inherit SKILL;

string type() { return "magic"; }
int valid_enable(string usage) { return usage == "magic"; }

/* 學習條件：道士或方士、等級 15（天風道長的任務，白雲觀 d/qiaoyin_east/npc/tianfong.c）。
 * 練成 20 級。 */
int valid_learn(object me)
{
    string cls = me->query("class");

    if( cls != "taoist" && cls != "alchemist" )
        return notify_fail("紫薇心經是道門與方士之間流傳的心法﹐你無緣修練。\n");
    if( me->query_level() < 15 )
        return notify_fail("你的根基尚淺﹐還參不透紫薇心經的玄機。\n");
    return 1;
}

int query_entry_level() { return 20; }

/* 學成只要 100 點經驗（使用者定案，2026-11-16：天風道長傳授給 100 點就等於學會）。 */
int query_entry_threshold() { return 100; }

void skill_completed(object me, string skill)
{
    tell_object(me, HIY "你將紫薇心經默誦再三﹐隱隱覺得周身有一層無形的光暈流轉。\n" NOR);
}

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
    if( kind == "freeze" ) return HIC;
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

string kind_attr(string kind)
{
    return ([ "fire": "火系", "storm": "風系", "freeze": "冰系", "thunder": "雷系" ])[kind];
}

/* 人人可見的施法敘述只有一行。 */
string cast_line(string kind)
{
    return "$N施放【" + kind_name(kind) + "】﹐周身靈光流轉﹐護得嚴嚴實實。";
}

string cancel_line(string kind)
{
    return "$N消除【" + kind_name(kind) + "】﹐周身的靈光應聲散去。";
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
        tell_object(me, kind_color(kind) + "你的" + kind_attr(kind) + "傷害防禦力降低了。\n" NOR);
}

void expire(object me, string kind, int id)
{
    remove_defence(me, kind, id, 0);
}

/* 施法點數（使用者定案，2026-11-16），每次施展成功各自擲一次，法術與紫薇心經都用靈性：
 * ((random(靈性)+1) × (random(靈性)+1) + random(靈性)) × (random(層級)+1)
 * 層級：全部是 1。 */
void magic_gain(object me, int tier)
{
    int spi, pts;

    if( !userp(me) ) return;
    spi = me->query_attr("spi");
    if( spi < 1 ) spi = 1;
    pts = ((random(spi) + 1) * (random(spi) + 1) + random(spi)) * (random(tier) + 1);
    me->improve_skill_exact("magic", pts);
    me->improve_skill_exact("chivi sutra", pts);
}

int spend_sen(object me, int cost)
{
    if( me->query_stat("sen") < cost )
        return notify_fail("你的心神不足﹐無法施展紫薇心經。\n");
    me->consume_stat("sen", cost);
    return 1;
}

/* 禦X術：消耗神 20；效果 = 紫薇心經等級，持續 紫薇心經/3 個 tick（最少 1）。
 * 同一系的禦術生效中不能重複施展，四系可以同時存在。 */
int do_defence(object me, string kind)
{
    int sk, bonus, ticks, id;

    if( me->query_temp("chivi/" + kind) )
        return notify_fail("你身上已經有" + kind_name(kind) + "的法力了。\n");
    if( !spend_sen(me, 20) ) return 0;

    sk = me->query_skill("chivi sutra", 1);
    bonus = sk;
    ticks = sk / 3;
    if( ticks < 1 ) ticks = 1;
    id = ++serial;

    me->add_temp("apply/" + armor_key(kind), bonus);
    me->set_temp("chivi/" + kind, ([ "bonus": bonus, "id": id ]));
    message_vision(kind_color(kind) + cast_line(kind) + "\n" NOR, me, 0, 1);
    tell_object(me, kind_color(kind) + "你的" + kind_attr(kind) + "傷害防禦力提高了。\n" NOR);
    call_out("expire", ticks * 2, me, kind, id);
    magic_gain(me, 1);
    return 1;
}

/* dispel：消耗神 5，把身上所有禦術的效果消除。 */
int do_dispel(object me)
{
    string *active = ({});

    foreach(string kind in ({ "fire", "storm", "freeze", "thunder" }))
        if( mapp(me->query_temp("chivi/" + kind)) ) active += ({ kind });
    if( !sizeof(active) )
        return notify_fail("你周身並沒有紫薇心經的保護。\n");
    if( !spend_sen(me, 5) ) return 0;

    foreach(string kind in active) {
        message_vision(kind_color(kind) + cancel_line(kind) + "\n" NOR, me, 0, 1);
        remove_defence(me, kind, 0, 0);
    }
    magic_gain(me, 1);
    return 1;
}

int conjure_magic(object me, string spell, object target)
{
    string kind;

    if( !me->query_skill("chivi sutra", 1) && !me->query_learn("chivi sutra") )
        return notify_fail("你還沒學會紫薇心經。\n");
    if( target && target != me )
        return notify_fail("紫薇心經只能保護自身﹐無法為他人施放。\n");
    if( spell == "dispel" ) return do_dispel(me);
    if( kind = kind_of(spell) ) return do_defence(me, kind);
    return notify_fail("指令格式﹕conjure <法術名稱>\n");
}
