// /d/snowkeep/entrance.c — 雪吟莊（D8）。依使用者提供的雪吟莊設計表（2026-11-06）；敘述：新寫。
// 區域文件見 docs/AREA_SNOWKEEP.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "雪吟莊大門");
    set("long", @LONG
山路到此豁然開朗，一座氣派的莊院倚山而建，朱漆大門上釘著兩排碗口大的銅釘，門楣高懸一塊黑底金字的匾額，寫著「雪吟山莊」四個大字。門前石階掃得一塵不染，往來的多是佩刀帶劍的江湖人物，個個神色倨傲。
LONG
    );
    set("exits", ([
        "west" : "/d/snow/ebridge",
        "east" : __DIR__"yard_w",
    ]));
    set("objects", ([
        "/d/snow/npc/guard" : 2,
    ]));
    set("map/area", "雪吟莊");
    setup();
    replace_program(ROOM);
}
