// /d/shuiyue/mountain_path2.c — 水月村（D34）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "上山小路");
    set("long", @LONG
山路至此變得平緩，路旁的樹叢中隱約看得見被踩出的小岔道，空氣裡帶著松脂與泥土混合的氣味。往來雪亭鎮的行人常在這裡歇腳，路邊的石塊被坐得光滑，像一張張天然的矮凳。
LONG
    );
    set("exits", ([
        "northup" : __DIR__"mountain_path1",
        "south" : "/d/snow/npath2",
    ]));
    set("map/area", "水月村");
    setup();
    replace_program(ROOM);
}
