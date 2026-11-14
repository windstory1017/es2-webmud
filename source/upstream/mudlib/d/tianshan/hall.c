// /d/tianshan/hall.c — 天山（V2）。

#include <room.h>

inherit ROOM;

int do_kneel(string arg)
{
    object me = this_player();

    if( arg != "靈位" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N忽然不見了。\n", me);
    if( !me->move("/d/tianshan/secret_room") ) return 0;
    message("vision", replace_string("$N忽然出現在牆邊。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void init()
{
    ::init();
    add_action("do_kneel", "kneel");
}

void create()
{
    set("short", "靈堂");
    set("long", @LONG
一張長桌上擺著一些供品，桌子的中央是一個靈位，上面刻著顯考柳公諱臻之靈位。屋子的另一端有幾把香，你可以跪拜靈位。
LONG
    );
    set("detail/靈位", "顯考柳公諱臻之靈位。");
    set("exits", ([
        "out" : __DIR__"cabin",
    ]));
    set("map/area", "天山");
    set("map/layer", "內堂");
    setup();
}
