// /d/swaylan/forest/forest2.c — 密林（AG23）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "密林");
    set("long", @LONG
幾棵大樹之間拉著麻繩，繩上掛著幾串風乾的獸肉，地上留著熄滅不久的火堆，灰燼裡還插著幾根啃剩的骨頭。這一帶常有人聚集，林間小徑被踩得結結實實，顯然不是尋常獵戶走出來的路。
LONG
    );
    set("exits", ([
        "west" : __DIR__"forest1",
        "east" : __DIR__"stele",
    ]));
    set("objects", ([
        __DIR__"npc/bandit" : 3,
    ]));
    set("map/area", "密林");
    setup();
    replace_program(ROOM);
}
