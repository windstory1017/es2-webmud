// /d/ziyan/lane.c — 紫煙鎮（O10）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "小街");
    set("long", @LONG
一條南北向的小街，東倒西歪的木屋擠在兩旁。西邊一戶人家的門口拉著麻繩，門板上還貼著官府的封條，左鄰右舍經過時都繞得遠遠的，彷彿那屋裡還住著什麼不乾淨的東西。
LONG
    );
    set("exits", ([
        "north" : __DIR__"corner",
        "south" : __DIR__"market",
        "west" : __DIR__"house",
    ]));
    set("map/area", "紫煙鎮");
    setup();
    replace_program(ROOM);
}
