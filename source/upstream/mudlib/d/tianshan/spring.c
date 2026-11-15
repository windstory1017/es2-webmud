// /d/tianshan/spring.c — 天山（N8）。

#include <room.h>

inherit ROOM;

int do_enter(string arg)
{
    object me = this_player();

    if( arg != "縫隙" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N鑽了進去。\n", me);
    if( !me->move("/d/tianshan/cave1") ) return 0;
    message("vision", replace_string("$N從外面鑽了進來。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void init()
{
    ::init();
    add_action("do_enter", "enter");
}

void create()
{
    set("short", "清泉");
    set("long", @LONG
一股清泉從對面的石壁中湧了出來，飛快的奔向山下。幾根藤條從石壁上垂了下來，落在泉口, 泉口的旁邊有一條縫隙。
LONG
    );
    set("detail/縫隙", "一條縫隙不知道自己能不能鑽進去 (enter)");
    set("exits", ([
        "south" : __DIR__"fog",
    ]));
    set("map/area", "天山");
    setup();
}
