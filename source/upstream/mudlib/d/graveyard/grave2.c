// /d/graveyard/grave2.c — 亂葬崗（N6）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "亂葬崗");
    set("long", @LONG
路旁的墳塚一座挨著一座，多半只是一抔黃土，連塊石碑也沒有，有些墳前還插著燒剩的半截香枝，想是仍有人記得來此祭拜。泥地上散著些被野狗刨出來的碎骨，白慘慘的，在亂草間格外刺眼。
LONG
    );
    set("outdoors", "snow");
    set("exits", ([
        "east" : __DIR__"grave1",
        "northwest" : __DIR__"grave3",
    ]));
    set("map/area", "亂葬崗");
    setup();
    replace_program(ROOM);
}
