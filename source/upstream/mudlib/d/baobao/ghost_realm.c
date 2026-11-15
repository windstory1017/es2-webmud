// /d/baobao/ghost_realm.c — 八寶樓（F6）。敘述：舊資料（刪一個「的」）。設計見 docs/AREA_BAOBAO.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "鬼境");
    set("long", @LONG
這裡異常陰暗潮濕，就好像地牢一樣。一種讓人反胃的血腥味充溢著整個幻境。幾具屍骨橫七豎八的躺在地上。你忽然覺得似乎有什麼人正在暗處盯你瞧。
LONG
    );
    set("exits", ([
        "north" : __DIR__"magic_realm",
    ]));
    set("objects", ([
        __DIR__"obj/skeleton" : 1,
        __DIR__"obj/corpse" : 1,
        "/custom/legacy_npc/gong_sun_wei" : 1,
    ]));
    set("map/area", "八寶樓");
    set("map/mode", "maze");
    setup();
    replace_program(ROOM);
}
