// /d/swaylan/buxuan/cliff_top.c — 落雲峰（AO10）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_climb", "climb");
}

int do_climb(string arg)
{
    object me = this_player();

    if( arg != "峭壁" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N攀著石壁上的落腳點，小心翼翼地爬了下去。\n", me);
    if( !me->move("/d/swaylan/buxuan/cliff") ) return 0;
    message("vision", replace_string("$N從峭壁上爬了下來。", "$N", me->name()) + "\n", environment(me), ({ me }));
    return 1;
}

void create()
{
    set("short", "山壁");
    set("long", @LONG
爬上峭壁，眼前是一片不大的平地，一面山壁上滿是深深淺淺的刻痕，縱橫交錯，像是被什麼利器反覆劃過。身後便是方才攀上來的峭壁，一旁蓋著一間小茅屋，看來有高人在此潛修。
LONG
    );
    set("detail", ([
        "山壁" : "在山壁上有許多的刻痕，是駱三少練習雲峰筆招所留下的痕跡。",
    ]));
    set("exits", ([
        "north" : __DIR__"hut",
    ]));
    set("map/area", "水嵐縣");
    setup();
}
