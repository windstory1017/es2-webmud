// /d/swaylan/forest/kitchen.c — 密林（AK25）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "廚房");
    set("long", @LONG
一間用木板搭成的廚房，灶上架著一口大鍋，鍋裡咕嘟咕嘟地燉著肉，香氣混著柴煙瀰漫了整間屋子。牆上掛著一排菜刀和砍骨的斧頭，案板上的血水還沒擦乾，看來這裡要餵飽的人可不少。
LONG
    );
    set("exits", ([
        "west" : __DIR__"clearing",
    ]));
    set("objects", ([
        __DIR__"npc/cooker" : 1,
    ]));
    set("map/area", "密林");
    setup();
    replace_program(ROOM);
}
