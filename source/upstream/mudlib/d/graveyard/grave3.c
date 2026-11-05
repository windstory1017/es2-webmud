// /d/graveyard/grave3.c — 亂葬崗（L4）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "亂葬崗");
    set("long", @LONG
這一帶地勢低窪，積著幾窪混濁的死水，水面上浮著一層暗綠的苔沫，散發出淡淡的腐臭。幾隻烏鴉蹲在一株枯死的老槐樹上，偶爾嘎嘎啼叫兩聲，又歸於沉寂，四下裏便只剩下蚊蚋嗡嗡的聲響。
LONG
    );
    set("outdoors", "snow");
    set("exits", ([
        "northwest" : __DIR__"grave4",
        "southeast" : __DIR__"grave2",
    ]));
    set("map/area", "亂葬崗");
    setup();
    replace_program(ROOM);
}
