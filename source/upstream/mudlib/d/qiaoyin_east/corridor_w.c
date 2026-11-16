// /d/qiaoyin_east/corridor_w.c — 白雲觀（D4）。依使用者設計表（2026-11-16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "西廊");
    set("long", @LONG
迴廊的木柱漆色斑駁，柱腳被往來衣擺磨得發亮。廊外的青磚地鋪得齊整，縫隙間長著幾叢細草。練武場那頭傳來的呼喝與拳腳聲，隔著廊壁變得悶悶的，倒讓這裡多了幾分肅靜。
LONG
    );
    set("exits", ([
        "north" : __DIR__"ground_w",
        "south" : __DIR__"sutra_hall",
        "west" : __DIR__"cell",
        "east" : __DIR__"court",
    ]));
    create_door("west", "房門", "east", DOOR_CLOSED);
    set("map/area", "白雲觀");
    setup();
    replace_program(ROOM);
}
