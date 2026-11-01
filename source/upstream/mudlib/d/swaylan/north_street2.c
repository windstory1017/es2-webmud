// /d/swaylan/north_street2.c — 水嵐縣（U14）。房間敘述照舊站原文。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "北大街");
    set("long", @LONG
走在街道上，你可以發現水嵐縣蠻熱鬧的，在街道上有很多行人走來走去，有小販在吆喝著，有些太太們出來買菜，向西北走去可以通往水嵐縣的第一大戶【冷梅莊】，向南望去可以看到有一家客棧，有很多的人聚集在那，你可以去看看有沒有什麼好吃的。
LONG
    );
    set("exits", ([
        "north" : __DIR__"north_street1",
        "south" : __DIR__"inn",
        "east" : __DIR__"smithy",
        "northwest" : __DIR__"crossroad",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
