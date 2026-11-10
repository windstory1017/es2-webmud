// /d/oldpine/cave_mouth.c — 迷霧森林（Z12）。房間敘述依設計表提示撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "山洞口");
    set("long", @LONG
獸徑的盡頭是一面陡峭的山壁，山壁下有一個黑漆漆的洞口，洞口的岩石被磨得光滑，顯然常有東西進出。洞口前散落著吃剩的蜂巢和魚骨，還有一些被扒開的泥土。洞裡隱約傳出低沉的鼾聲和咕嚕聲，讓人遲疑該不該進去....
LONG
    );
    set("exits", ([
        "west" : __DIR__"beast_trail",
    ]));
    set("map/area", "迷霧森林");
    setup();
}
