// /d/swaylan/gambling_house.c — 水嵐縣（Q14）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "賭場");
    set("long", @LONG
一間剛被官府查封的賭場，門板上還交叉貼著蓋了大印的封條，被人撕開了一角。屋裡的賭桌翻倒在地，骰子和竹籌撒了一地也沒人收拾，空氣中殘留著汗臭與劣酒的氣味，一片狼藉。
LONG
    );
    set("exits", ([
        "south" : __DIR__"west_gate",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
