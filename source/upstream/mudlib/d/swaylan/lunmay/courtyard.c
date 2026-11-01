// /d/swaylan/lunmay/courtyard.c — 冷梅莊（I23）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "冷梅莊");
    set("long", @LONG
一片老梅環繞的庭院，碎石小徑在梅林間蜿蜒，連接著前後院落。梅樹都有些年歲了，枝幹虯曲如鐵，樹下落著厚厚一層枯葉，劍刃交擊的清響不時從一旁的練武場傳來，鏗鏘悅耳。
LONG
    );
    set("exits", ([
        "north" : __DIR__"hall",
        "south" : __DIR__"front_yard",
        "west" : __DIR__"pavilion",
        "east" : __DIR__"drill_n",
    ]));
    set("map/area", "冷梅莊");
    setup();
    replace_program(ROOM);
}
