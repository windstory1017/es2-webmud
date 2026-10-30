// /d/swaylan/street_se.c — 水嵐縣（AC18）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "街道");
    set("long", @LONG
城東南角一條街道，兩旁是幾戶殷實人家，粉牆黛瓦，牆頭探出幾枝石榴。不時有婦人提著菜籃經過，在井邊停下來閒話家常，說笑聲隔著老遠便能聽見，透著一股安穩的氣息。
LONG
    );
    set("exits", ([
        "north" : __DIR__"east_gate",
        "southwest" : __DIR__"alley",
    ]));
    set("objects", ([
        __DIR__"npc/woman" : 1,
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
