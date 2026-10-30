// /d/ziyan/narrow_street.c — 紫煙鎮（I8）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_enter", "enter");
}

int do_enter(string arg)
{
    object me = this_player();

    if( arg != "茅屋" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N循著地上的血跡，推開茅屋的破門走了進去。\n", me);
    if( !me->move("/d/ziyan/hut") ) return 0;
    message("vision", replace_string("$N推開破門走了進來。", "$N", me->name()) + "\n", environment(me), ({ me }));
    return 1;
}

void create()
{
    set("short", "窄街");
    set("long", @LONG
一條狹窄的街道，兩旁的屋舍低矮破舊，北邊有一間孤零零的茅屋。地上撒了一層新土，像是有人匆忙掩蓋過什麼，泥土底下卻隱約透出幾點暗紅的血跡，斷斷續續地一路延伸到茅屋門口。
LONG
    );
    set("exits", ([
        "east" : __DIR__"street",
        "west" : __DIR__"yamen_street",
    ]));
    set("map/area", "紫煙鎮");
    setup();
}
