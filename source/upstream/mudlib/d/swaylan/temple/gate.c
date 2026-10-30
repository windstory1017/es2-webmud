// /d/swaylan/temple/gate.c — 寶蓮寺（AA14）。舊資料全文引用。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "寶蓮寺");
    set("long", @LONG
來到這裡，映入你眼簾的是一座廟宇，裡面有很多的進香客，這裡恭奉的是觀世音菩薩，在前方有一座觀世音菩薩的塑身，前面擺了很多的供桌，上面有許多的香，水果，食物，在空中飄著裊裊香煙，旁邊有廟祝在那裡為進香客祈福。
LONG
    );
    set("exits", ([
        "south" : "/d/swaylan/east_street3",
        "northeast" : __DIR__"side_hall",
    ]));
    set("objects", ([
        __DIR__"npc/abbot" : 1,
    ]));
    set("no_fight", 1);
    set("map/area", "寶蓮寺");
    setup();
    replace_program(ROOM);
}
