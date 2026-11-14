// /d/tianshan/cave_mouth.c — 天山（T8）。

#include <room.h>

inherit ROOM;

int do_search(string arg)
{
    object me = this_player();

    if( arg != "小徑" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N消失在桃花林中。\n", me);
    if( !me->move("/d/tianshan/dirt_road1") ) return 0;
    message("vision", replace_string("$N從桃花叢中走了出來。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void init()
{
    ::init();
    add_action("do_search", "search");
}

void create()
{
    set("short", "洞口");
    set("long", @LONG
洞外是一片茂盛的桃林，林中不停的飄來陣陣醉人的花香。你注意到這片桃林是經過精心照顧，好像是有什麼人故意做的迷陣。桃花叢中好像有一條小徑。
LONG
    );
    set("detail/桃花叢", "桃花叢中好像有一條小徑, 你可以試著找找 (search) 小徑");
    set("exits", ([
        "west" : __DIR__"cave2",
    ]));
    set("map/area", "天山");
    setup();
}
