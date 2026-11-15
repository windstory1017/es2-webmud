// /d/baobao/illusion_realm.c — 八寶樓（D4）。敘述：舊資料（刪一個「的」）。設計見 docs/AREA_BAOBAO.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "幻境");
    set("long", @LONG
房間的正中央堆著整整三十個骷髏頭，每個骷髏頭上塗著一種白色的粉末。骷髏塔的四周點著一圈蠟燭，看起來這間屋子的主人似乎是用毒的高手。
LONG
    );
    set("exits", ([
        "up" : __DIR__"top_floor",
    ]));
    set("objects", ([
        __DIR__"npc/shi_yu" : 1,
    ]));
    set("map/area", "八寶樓");
    set("map/mode", "maze");
    setup();
    replace_program(ROOM);
}
