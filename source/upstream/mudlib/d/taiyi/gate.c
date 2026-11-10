// /d/taiyi/gate.c — 太乙觀門口（L16）。房間敘述依設計表需求撰寫（docs/AREA_TAIYI.md）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "太乙觀門口");
    set("long", @LONG
山路到了這裡豁然開朗，一座青瓦道觀倚著山勢而建，朱漆大門兩側各蹲著一尊石獅，鬃毛被往來的香客摸得發亮。門內香煙裊裊，鐘磬聲隱隱傳出，進出的人流絡繹不絕，可見此間香火之盛。
LONG
    );
    set("outdoors", "snow");
    set("exits", ([
        "northwest" : __DIR__"hall",
        "east" : "/d/graveyard/grave8",
    ]));
    set("objects", ([
        "/d/baihua/npc/stone_lion" : 2,
    ]));
    set("map/area", "太乙觀");
    setup();
    replace_program(ROOM);
}
