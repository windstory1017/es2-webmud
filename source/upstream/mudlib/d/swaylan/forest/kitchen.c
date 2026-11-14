// /d/swaylan/forest/kitchen.c — 密林（AK25）。房間敘述依設計表需求撰寫；2026-11-01 改為地底的灶房。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "廚房");
    set("long", @LONG
空地旁挖出的一間灶房，四壁是夯實的泥土，灶上架著一口大鍋，鍋裡咕嘟咕嘟地燉著肉。灶頂開了一道斜斜的煙道通往地面，柴煙仍嗆得人直咳，牆上掛著一排菜刀和砍骨的斧頭，看來這裡要餵飽的人可不少。
LONG
    );
    set("exits", ([
        "west" : __DIR__"clearing",
    ]));
    set("objects", ([
        __DIR__"npc/cooker" : 1,
    ]));
    set("map/area", "密林");
    set("map/layer", "地下");   // 推石碑掉下來的地底（2026-11-01）
    setup();
    replace_program(ROOM);
}
