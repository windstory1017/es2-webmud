// /d/fengcheng/mountain_road.c — 風城（J12）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "山路");
    set("long", @LONG
一條平坦整齊的山路﹐鋪路所用的石料當然是出自天山山脈。從石階的光滑程度來看﹐這條石路至少已經用了幾百年了。山路的北面似乎有一座道觀在雲霧中若隱若現。
LONG
    );
    set("exits", ([
        "north" : __DIR__"qingyun_temple",
        "east" : __DIR__"east_gate",
    ]));
    set("map/area", "風城");
    setup();
    replace_program(ROOM);
}
