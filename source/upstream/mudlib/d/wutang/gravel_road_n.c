// /d/wutang/gravel_road_n.c — 五堂鎮（H12）。房間敘述依設計表需求擴寫。

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
    if( !me->move("/d/wutang/bamboo_grove") ) return 0;
    message("vision", replace_string("$N撥開草叢，從另一頭鑽了出來。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void create()
{
    set("short", "碎石路");
    set("long", @LONG
一條鋪滿碎石的小路，走起來沙沙作響。西側不遠就是羿水河邊，路旁長著一大叢茂密的草叢，旁邊還有一片竹蘆，風一吹便沙沙作響，似乎可以從草叢穿過去。往東是五堂鎮的街道，往北是菜田前的空地。
LONG
    );
    set("exits", ([
        "enter" : __DIR__"pavilion",
        "east" : __DIR__"west_street2",
        "north" : __DIR__"clearing",
        "southwest" : __DIR__"gravel_road_s",
    ]));
    set("map/area", "五堂鎮");
    setup();
}
