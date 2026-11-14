// /d/fengcheng/qingyun_temple.c — 風城（J10）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "青雲觀");
    set("long", @LONG
觀門不大，青瓦覆頂，門楣上「青雲觀」三字蒼勁古樸，被山霧浸得微微發暗。院裡幾株老柏撐開濃蔭，爐中殘香裊裊，混著遠處隱約的誦經聲。天師四破雷法的傳承便在此處，江湖中人雖少往來，山下卻無人不知。
LONG
    );
    set("exits", ([
        "south" : __DIR__"mountain_road",
    ]));
    set("objects", ([
        __DIR__"npc/hua_zi_yi" : 1,
    ]));
    set("map/area", "風城");
    setup();
    replace_program(ROOM);
}
