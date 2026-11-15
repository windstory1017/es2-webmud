// /d/fengcheng/stone_road2.c — 風城（L14）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "石板路");
    set("long", @LONG
石板路微微向上傾斜，路旁立著一排老槐，枝葉把路面遮得斑駁。越往城門走，人聲越稀，只剩車輪碾過石板的悶響，與山風吹過牆縫的嗚咽，教人不自覺把腳步放慢。
LONG
    );
    set("exits", ([
        "north" : __DIR__"east_gate",
        "south" : __DIR__"east_street2",
    ]));
    set("objects", ([
        "/obj/area/man" : 1,
    ]));
    set("map/area", "風城");
    setup();
    replace_program(ROOM);
}
