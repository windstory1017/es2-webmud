// /d/swaylan/lunmay/corridor_e.c — 冷梅莊（K21）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "穿廊");
    set("long", @LONG
東邊的穿廊比別處安靜得多，往來的僕役走到這裡都放輕了腳步，彷彿怕驚擾了什麼。廊邊擺著幾只熬藥用的陶爐，爐底的炭灰還有餘溫，空氣中隱隱飄著一股苦澀的藥氣。
LONG
    );
    set("exits", ([
        "north" : __DIR__"bedroom",
        "west" : __DIR__"hall",
    ]));
    set("map/area", "冷梅莊");
    create_door("north", "木門", "south", 0);   // 平時開著，可以關上
    setup();
}
