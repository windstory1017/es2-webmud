// /d/kangping/valley_mouth.c — 康平村（H4）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "谷口");
    set("long", @LONG
康平村的北口，再往北走便是三煙谷，谷中終年飄著三道淡淡的煙霧，因而得名。近來谷裡常有野狼出沒，夜裡狼嚎聲此起彼落，村民在路口立了一道木柵，天黑之後便不讓人進谷了。
LONG
    );
    set("exits", ([
        "west" : __DIR__"village",
        "east" : __DIR__"inn",
        "south" : __DIR__"square",
    ]));
    set("outdoors", "kangping");
    set("map/area", "康平村");
    setup();
    replace_program(ROOM);
}
