// /d/wutang/boat.c — 五堂鎮（D16）。房間敘述依設計表需求擴寫。

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
    message_vision("$N撲通一聲跳進河裡，往岸邊游了回去。\n", me);
    if( !me->move("/d/wutang/riverside") ) return 0;
    message("vision", replace_string("$N渾身溼淋淋地從河裡爬上了岸。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void create()
{
    set("short", "小船上");
    set("long", @LONG
一艘用稻草搭著篷子的小草船，靜靜地停在羿水河心，隨著水波緩緩搖晃。船頭擱著一根釣竿，船上的人似乎已經在這裡垂釣了很久。四周都是河水，想回岸邊只能游回去。
LONG
    );
    set("objects", ([
        __DIR__"npc/smoke_fisher" : 1,
    ]));
    set("map/area", "五堂鎮");
    set("detail/河水", "船外盡是滔滔的河水，想回岸邊的話，只能跳下水游 (swim) 回去。");
    setup();
}
