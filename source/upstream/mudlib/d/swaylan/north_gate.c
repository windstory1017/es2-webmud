// /d/swaylan/north_gate.c — 水嵐縣（U10）。房間敘述依設計表需求撰寫。

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

    if( arg != "破洞" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N彎下身子，從牆根的破洞鑽了進去。\n", me);
    if( !me->move("/d/swaylan/old_ferry") ) return 0;
    message("vision", replace_string("$N從牆上的破洞鑽了進來，拍了拍身上的塵土。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void create()
{
    set("short", "北城門");
    set("long", @LONG
水嵐縣的北城門正對著羿水，城樓不高，門洞卻砌得厚實，幾名守衛持著兵器分立兩旁，盤查進出的行人。城門東首緊挨著一段年久失修的舊牆，牆根坍出一個半人高的破洞，洞後隱約是一片荒廢的院落。
LONG
    );
    set("exits", ([
        "north" : __DIR__"ferry",
        "south" : __DIR__"north_street1",
    ]));
    set("objects", ([
        __DIR__"npc/gate_guard" : 4,
    ]));
    set("map/area", "水嵐縣");
    setup();
}
