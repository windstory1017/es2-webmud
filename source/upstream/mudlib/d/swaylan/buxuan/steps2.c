// /d/swaylan/buxuan/steps2.c — 步玄派（AQ14）。房間敘述依設計表需求撰寫。

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

    if( arg != "大廳" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N推開半掩的廳門，走進了大廳。\n", me);
    if( !me->move("/d/swaylan/buxuan/residence") ) return 0;
    message("vision", replace_string("$N推門走了進來。", "$N", me->name()) + "\n", environment(me), ({ me }));
    return 1;
}

void create()
{
    set("short", "石階");
    set("long", @LONG
石階到了這裡已是盡頭，前方一座清幽的大廳依山而建，廳門半掩，門楣上的匾額字跡清雅，隱約透出一縷檀香。階邊立著兩盞石燈，燈身爬滿了青苔，看來已有不少年歲。
LONG
    );
    set("exits", ([
        "west" : __DIR__"terrace",
        "south" : __DIR__"field2",
        "north" : __DIR__"peak_foot",
    ]));
    set("objects", ([
        __DIR__"npc/apprentice" : 2,
    ]));
    set("map/area", "步玄派");
    setup();
}
