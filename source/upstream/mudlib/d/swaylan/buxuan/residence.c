// /d/swaylan/buxuan/residence.c — 步玄派（AS14）。舊站「沐華居」全文引用。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "沐華居");
    set("long", @LONG
這裡是步玄派掌門人的居所沐華居，室內的擺設多半是年代相當久遠的紅木家具，一方茶几上燃著一爐檀香。廳堂上懸著一幅步玄派始祖駱步玄的字畫，畫著一頭顧盼生威的大鷹。
LONG
    );
    set("exits", ([
        "out" : __DIR__"steps2",
    ]));
    set("objects", ([
        "/custom/legacy_npc/ro_yi" : 1,
        __DIR__"npc/apprentice" : 2,
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
