// /d/mengjia/manor.c — 孟家莊（B10）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "孟家莊");
    set("long", @LONG
宅院門庭開闊，青磚鋪地，正堂前懸著「五虎傳家」的匾額，筆力雄渾，邊角已被歲月磨得發暗。兩旁廊柱上掛著幾口舊刀，刀身擦得雪亮，孟家雖更像武林世家而非軍門，家傳的武功卻從未放下。
LONG
    );
    set("exits", ([
        "north" : __DIR__"backyard",
        "south" : __DIR__"gate",
    ]));
    set("objects", ([
        "/custom/legacy_npc/meng_jiang_hong" : 1,
    ]));
    set("map/area", "孟家莊");
    setup();
    replace_program(ROOM);
}
