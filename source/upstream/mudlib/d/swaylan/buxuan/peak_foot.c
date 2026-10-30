// /d/swaylan/buxuan/peak_foot.c — 落雲峰（AQ12）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "落雲峰底");
    set("long", @LONG
落雲峰的山腳下，峰頂終年隱沒在雲霧之中，只看得見半截青黑的山身。山風從峰上吹下來，帶著幾分涼意，四下一片寂靜，偶爾傳來幾聲鷹唳，在山谷間久久迴盪不去。
LONG
    );
    set("exits", ([
        "west" : __DIR__"cliff",
        "south" : __DIR__"steps2",
    ]));
    set("map/area", "落雲峰");
    setup();
    replace_program(ROOM);
}
