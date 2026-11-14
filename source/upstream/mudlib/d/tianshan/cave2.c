// /d/tianshan/cave2.c — 天山（R8）。

#include <room.h>

inherit ROOM;

int do_enter(string arg)
{
    object me = this_player();

    if( arg != "洞口" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N忽然不見了。\n", me);
    if( !me->move("/d/tianshan/cave_mouth") ) return 0;
    message("vision", replace_string("$N從洞裡鑽了出來。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void init()
{
    ::init();
    add_action("do_enter", "enter");
}

void create()
{
    set("short", "山洞內");
    set("long", @LONG
山洞裡黑黑的，只有幾絲光亮從西邊的一個小洞口照射進來。
LONG
    );
    set("detail/洞口", "有一個洞口, 你可以試著鑽進去 (enter)");
    set("exits", ([
        "west" : __DIR__"cave1",
    ]));
    set("map/area", "天山");
    set("map/layer", "洞穴");
    setup();
}
