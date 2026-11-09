// /d/taiyi/guest3.c — 香客廂房（F14）。房間敘述舊資料全文引用（賓客宿房）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "香客廂房");
    set("long", @LONG
這是一間淡雅的素房, 整個房間有一種說不出來的寧靜和無慾。中央
的小桌上一面小旗寫著: 太乙觀宿房。
LONG
    );
    set("exits", ([
        "east" : __DIR__"corridor1",
    ]));
    set("objects", ([
        __DIR__"npc/pilgrim" : 1,
    ]));
    create_door("east", "廂房木門", "west", DOOR_CLOSED);
    set("map/area", "太乙觀");
    setup();
    replace_program(ROOM);
}
