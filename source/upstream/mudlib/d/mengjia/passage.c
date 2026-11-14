// /d/mengjia/passage.c — 孟家莊（暗道）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "暗道");
    set("long", @LONG
一條狹窄的通道，一般人低著頭剛好走得過去。只是，這條密道究竟通往何出處呢？
LONG
    );
    set("exits", ([
        "up" : __DIR__"inn2",
        "north" : __DIR__"forest",
    ]));
    set("objects", ([
        "/custom/legacy_npc/hua_xing" : 1,
    ]));
    set("map/area", "孟家莊");
    set("map/layer", "暗道");
    setup();
    replace_program(ROOM);
}
