// /d/graveyard/weeds.c — 亂葬崗（D8）。房間敘述舊資料全文引用。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "雜草叢");
    set("long", @LONG
這裡是亂葬崗上的一處雜草叢中﹐許多蚊蟲在你的身邊飛來飛去﹐
使你覺得很不自在﹐你的南邊是一處密林﹐看起來陰森森的﹐如果你想
離開這裡﹐往北穿過草叢可以回到小路上。
LONG
    );
    set("outdoors", "snow");
    set("exits", ([
        "north" : __DIR__"grave8",
        "south" : __DIR__"forest",
    ]));
    set("map/area", "亂葬崗");
    setup();
    replace_program(ROOM);
}
