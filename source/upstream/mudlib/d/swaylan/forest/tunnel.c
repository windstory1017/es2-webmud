// /d/swaylan/forest/tunnel.c — 密林（AI27）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "地下密道");
    set("long", @LONG
一條挖在地底的密道，四壁用木樁撐著，頭頂不時落下細碎的泥土。壁上每隔幾步插著一支火把，火光搖曳，把人影拉得忽長忽短，空氣又悶又潮，混著一股汗臭與煤油的氣味。
LONG
    );
    set("exits", ([
        "up" : __DIR__"stele",
        "southwest" : __DIR__"hideout",
    ]));
    set("objects", ([
        __DIR__"npc/bandit" : 2,
        __DIR__"npc/bandit_leader" : 1,
    ]));
    set("map/area", "密林");
    set("map/layer", "地下");
    setup();
    replace_program(ROOM);
}
