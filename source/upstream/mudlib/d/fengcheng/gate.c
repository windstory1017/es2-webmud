// /d/fengcheng/gate.c — 風城（D20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "城門口");
    set("long", @LONG
高大的城門洞開，城牆上「風城」兩個大字被風沙磨得有些模糊，守衛來回踱步，目光不時掃過進出的行商。城門旁一道石階貼著牆壁盤旋而上，沒入高處的城樓，階面上一層薄薄的塵土。
LONG
    );
    set("exits", ([
        "north" : __DIR__"south_street",
        "south" : "/d/shuiyue/stone_road",
        "up" : __DIR__"tower",
    ]));
    set("map/area", "風城");
    setup();
    replace_program(ROOM);
}
