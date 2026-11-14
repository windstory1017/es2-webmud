// /d/wutang/bamboo_grove.c — 五堂鎮（H16）。房間敘述依設計表需求擴寫。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_pass", "pass");
}

int do_pass(string arg)
{
    object me = this_player();

    if( arg != "草叢" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N撥開草叢，鑽了進去。\n", me);
    if( !me->move("/d/wutang/gravel_road_n") ) return 0;
    message("vision", replace_string("$N撥開草叢，從另一頭鑽了出來。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void create()
{
    set("short", "綠竹林");
    set("long", @LONG
一片茂密的綠竹林，修長的竹子直入雲霄，竹葉沙沙作響，陽光從縫隙間灑落下來，顯得格外清幽。往北隱約可以看見一間綠竹搭成的小屋，往回走則要穿過那一大叢草叢。
LONG
    );
    set("detail/草叢", "回去的路被一大叢草叢擋住，撥開草叢可以穿過 (pass) 去。");
    set("exits", ([
        "north" : __DIR__"bamboo_hall",
    ]));
    set("map/area", "五堂鎮");
    setup();
}
