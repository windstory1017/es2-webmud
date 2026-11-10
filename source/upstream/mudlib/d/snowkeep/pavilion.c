// /d/snowkeep/pavilion.c — 雪吟莊（F4）涼亭。依使用者提供的雪吟莊設計表（2026-11-06）；敘述：新寫。
// 八卦桌機關（press <卦位>）：每次開機決定一組八卦順序，該次開機固定不變。
//   按對：銅鈕沉下；八個依序按對，亭中石階打開 450 tick，通往地牢（down）。
//   按錯：射出短箭，氣的上限受傷 10 點；之後在按對之前每再錯一次多傷 5 點；已沉下的銅鈕全部彈回。
//   按到已經沉下的銅鈕：只顯示已沉下，不重算受傷。
// 區域文件見 docs/AREA_SNOWKEEP.md。

#include <ansi.h>
#include <room.h>

inherit ROOM;

#define TICK        2       // 一個 tick 兩秒
#define OPEN_TICKS  450     // 石階打開多久
#define HURT_FIRST  10      // 第一次按錯傷氣上限
#define HURT_MORE   5       // 按對之前再錯，每次多傷

private string *GUA = ({ "乾", "坤", "震", "艮", "離", "坎", "兌", "巽" });
private string *order;

void init()
{
    ::init();
    add_action("do_press", "press");
}

string *query_order() { return copy(order); }

void close_stairs()
{
    if( !query("exits/down") ) return;
    delete("exits/down");
    tell_room(this_object(), "只聽得軋軋聲響﹐亭中的石板緩緩移回原處﹐將往下的石階蓋得嚴嚴實實。\n");
}

private void open_stairs()
{
    set("exits/down", __DIR__"dungeon_w");
    tell_room(this_object(), HIY "八枚銅鈕全數沉落﹐亭中地面一塊石板軋軋移開﹐露出一道往下的石階。\n" NOR);
    remove_call_out("close_stairs");
    call_out("close_stairs", OPEN_TICKS * TICK);
}

int do_press(string arg)
{
    object me = this_player();
    int step, idx, hurt;

    if( !arg ) return notify_fail("你要按哪一個卦位的銅鈕﹖\n");
    arg = replace_string(arg, " ", "");
    arg = replace_string(arg, "位", "");
    if( member_array(arg, GUA) == -1 )
        return notify_fail("八卦桌上沒有這個卦位。\n");
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    if( query("exits/down") )
        return notify_fail("八枚銅鈕都已經沉了下去﹐亭中的石階正敞開著。\n");

    step = me->query_temp("snowkeep/bagua_step");
    idx = member_array(arg, order);

    // 已經沉下的銅鈕：不重算受傷。
    if( idx < step ) {
        write("「" + arg + "」位的銅鈕已經沉下去了。\n");
        return 1;
    }

    // 按對。
    if( idx == step ) {
        me->delete_temp("snowkeep/bagua_hurt");
        message_vision("$N按下「" + arg + "」位的銅鈕﹐只聽得喀的一聲輕響﹐銅鈕緩緩沉了下去。\n", me, 0, 1);
        if( ++step >= sizeof(order) ) {
            me->delete_temp("snowkeep/bagua_step");
            open_stairs();
        } else
            me->set_temp("snowkeep/bagua_step", step);
        return 1;
    }

    // 按錯：受傷遞增，已沉下的銅鈕全部彈回。
    hurt = me->query_temp("snowkeep/bagua_hurt");
    hurt = hurt ? hurt + HURT_MORE : HURT_FIRST;
    me->set_temp("snowkeep/bagua_hurt", hurt);
    me->delete_temp("snowkeep/bagua_step");
    message_vision(HIR "$N按下「" + arg + "」位的銅鈕﹐桌下忽然嗤的一聲射出一支短箭﹐正中$N的身子！\n" NOR, me, 0, 1);
    if( step > 0 )
        tell_object(me, "只聽得喀喀幾聲﹐原本沉下去的銅鈕全都彈了回來。\n");
    me->damage_stat("kee", hurt);
    return 1;
}

void create()
{
    int i, j;
    string t;

    set("short", "涼亭");
    set("long", @LONG
這是一座八角涼亭，亭頂覆著青瓦，八根朱紅亭柱上各刻著一個卦象。亭中擺著一張石造的八卦桌，桌面依卦位刻出八道紋路，每道紋路的盡頭都嵌著一枚微微凸起的銅鈕，摸上去冰涼光滑，不像是尋常的擺設。
LONG
    );
    set("detail/八卦桌",
        "一張青石雕成的八卦桌，桌緣依乾、坤、震、艮、離、坎、兌、巽八個卦位各嵌著一枚略略凸起的銅鈕，銅鈕四周磨得發亮，似乎常有人去按 (press) 它。");
    set("detail/銅鈕", query("detail/八卦桌"));
    set("exits", ([
        "south" : __DIR__"path",
    ]));
    set("map/area", "雪吟莊");
    set("no_clean_up", 1);   // 順序要整次開機固定，房間不能被清掉重載

    // 每次開機決定一組順序。
    order = copy(GUA);
    for( i = sizeof(order) - 1; i > 0; i-- ) {
        j = random(i + 1);
        t = order[i]; order[i] = order[j]; order[j] = t;
    }
    setup();
}
