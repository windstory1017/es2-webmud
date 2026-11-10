// /d/graveyard/grave7.c — 亂葬崗（F6）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "亂葬崗");
    set("long", @LONG
坡腳下有一座破敗的石亭，亭頂缺了好幾片瓦，石柱上的漆早已剝落殆盡。亭旁幾座新墳的黃土還未長出青草，低低的誦經聲在墳間迴盪，木魚篤篤，一聲接著一聲，像是要替這些無主的亡魂求個安穩。
LONG
    );
    set("outdoors", "snow");
    set("exits", ([
        "north" : __DIR__"grave6",
        "west" : __DIR__"grave8",
    ]));
    set("objects", ([
        __DIR__"npc/old_bonze" : 1,
        __DIR__"npc/young_bonze" : 1,
    ]));
    set("map/area", "亂葬崗");
    setup();
    replace_program(ROOM);
}
