// /d/ziyan/herb_yard.c — 紫煙鎮（E4）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "後院");
    set("long", @LONG
藥鋪後頭的一方小院，地上擺滿了竹篩，篩裡攤著切成薄片的藥材，曬得乾巴巴的。院角堆著幾口陶甕，濃烈的藥味混著泥土氣息撲鼻而來，一扇小門半掩著，門後便是前頭的藥鋪。
LONG
    );
    set("exits", ([
        "in" : __DIR__"drugstore",
    ]));
    set("objects", ([
        __DIR__"npc/herb_boy" : 2,
    ]));
    set("map/area", "紫煙鎮");
    setup();
    replace_program(ROOM);
}
