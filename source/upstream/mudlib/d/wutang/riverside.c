// /d/wutang/riverside.c — 五堂鎮（D14）。房間敘述照設計表原文。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_swim", "swim");
}

int do_swim(string arg)
{
    object me = this_player();

    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N撲通一聲跳進河裡，往河心的小船游了過去。\n", me);
    if( !me->move("/d/wutang/boat") ) return 0;
    message("vision", replace_string("$N渾身溼淋淋地從河裡爬上了小船。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void create()
{
    set("short", "河邊");
    set("long", @LONG
這裏是羿水河邊, 西邊是一座大山, 陡峭的山壁隱隱現出不凡的氣勢, 東邊則是通往五堂鎮的石道, 商旅不時的來來往往。
LONG
    );
    set("detail/河水", "羿水河面寬闊，水流不急，河心隱約泊著一艘小草船，水性好的人可以游 (swim) 過去。");
    set("detail/羿水", "羿水河面寬闊，水流不急，河心隱約泊著一艘小草船，水性好的人可以游 (swim) 過去。");
    set("exits", ([
        "east" : __DIR__"gravel_road_s",
    ]));
    set("objects", ([
        __DIR__"npc/ro" : 1,
        __DIR__"npc/huang" : 1,
    ]));
    set("no_fight", 1);
    set("map/area", "五堂鎮");
    setup();
}
