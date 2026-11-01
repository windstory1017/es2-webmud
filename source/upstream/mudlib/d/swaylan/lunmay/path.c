// /d/swaylan/lunmay/path.c — 冷梅莊（I27）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "冷梅莊");
    set("long", @LONG
進了大門，一條青石甬道筆直通往前院，兩旁種滿了梅樹，一株挨著一株，枝椏在頭頂交錯成一道天然的拱廊。腳步踏在甬道上，四下清幽寂靜，教人不知不覺放慢了步子。
LONG
    );
    set("exits", ([
        "north" : __DIR__"front_yard",
        "south" : __DIR__"gate",
    ]));
    set("map/area", "冷梅莊");
    setup();
    replace_program(ROOM);
}
