// /d/swaylan/lunmay/gate.c — 冷梅莊（I29）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "冷梅莊大門");
    set("long", @LONG
冷梅莊的大門是兩扇黑漆木門，門上銅環擦得發亮，門楣懸著一塊匾額，上書「冷梅莊」三個大字，筆力蒼勁。門前的石階掃得一塵不染，牆頭探出幾枝老梅，和莊外的塵土喧囂恍如兩個天地。
LONG
    );
    set("exits", ([
        "north" : __DIR__"path",
        "south" : "/d/swaylan/crossroad",
    ]));
    set("objects", ([
        __DIR__"npc/apprentice3" : 2,
    ]));
    set("map/area", "冷梅莊");
    setup();
    replace_program(ROOM);
}
