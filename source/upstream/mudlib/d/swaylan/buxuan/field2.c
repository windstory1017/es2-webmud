// /d/swaylan/buxuan/field2.c — 步玄派（AQ16）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "農田");
    set("long", @LONG
這一片田地種的多是菜蔬，畦壟分明，蘿蔔、青菜一行行排得整整齊齊。田頭挖了一道引水的溝渠，山泉從高處汩汩流下，水聲清脆，田邊一架水車吱呀吱呀地慢慢轉著。
LONG
    );
    set("exits", ([
        "west" : __DIR__"field1",
        "north" : __DIR__"steps2",
    ]));
    set("objects", ([
        __DIR__"npc/farmer" : 2,
    ]));
    set("map/area", "步玄派");
    setup();
    replace_program(ROOM);
}
