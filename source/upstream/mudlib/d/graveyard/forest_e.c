// /d/graveyard/forest_e.c — 亂葬崗（F10）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "密林");
    set("long", @LONG
林間一片被踩得光禿禿的空地，中央用石塊圍了個火坑，坑裏積著黑黑的炭灰，四周散落著獸骨與斷折的木矛。幾株大樹之間用藤蔓架起了簡陋的棚子，棚下堆著些獸皮，一股濃重的羶腥味撲鼻而來，令人作嘔。
LONG
    );
    set("outdoors", "snow");
    set("exits", ([
        "west" : __DIR__"forest",
    ]));
    set("objects", ([
        "/custom/legacy_npc/barbarian_chief" : 1,
        __DIR__"npc/barbarian" : 2,
    ]));
    set("map/area", "亂葬崗");
    setup();
    replace_program(ROOM);
}
