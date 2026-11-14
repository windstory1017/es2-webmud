// /d/tianshan/fog.c — 天山（N10）。

#include <room.h>

inherit ROOM;

int do_search(string arg)
{
    object me = this_player();

    if( arg != "山路" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N的身影緩緩的消失在霧的深處。\n", me);
    if( !me->move("/d/tianshan/spring") ) return 0;
    message("vision", replace_string("$N從霧中走了出來。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void init()
{
    ::init();
    add_action("do_search", "search");
    add_action("do_search", "msearch");
}

void create()
{
    set("short", "霧海");
    set("long", @LONG
這裡的霧氣十分沉重，你差點連回去的路都找不到，更別提上山的山路了。
LONG
    );
    set("exits", ([
        "westdown" : "/d/fengcheng/east_gate",
    ]));
    set("map/area", "天山");
    setup();
}
