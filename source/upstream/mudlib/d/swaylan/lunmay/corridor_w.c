// /d/swaylan/lunmay/corridor_w.c — 冷梅莊（G21）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "穿廊");
    set("long", @LONG
一道木造的穿廊連著正廳與西側的廳房，廊柱漆成深褐色，欄杆外種著一排老梅，枝幹斜斜探進廊裡。廊下鋪著青磚，被往來的腳步磨得光滑，走在上面，腳步聲在廊間輕輕迴盪。
LONG
    );
    set("exits", ([
        "north" : __DIR__"side_hall",
        "east" : __DIR__"hall",
    ]));
    set("map/area", "冷梅莊");
    create_door("north", "木門", "south", 0);   // 平時開著，可以關上
    setup();
}
