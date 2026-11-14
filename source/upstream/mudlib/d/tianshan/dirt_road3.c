// /d/tianshan/dirt_road3.c — 天山（X4）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "土路");
    set("long", @LONG
土路到這裡更窄了，兩旁立著幾根斑駁的木樁，上頭拴過的麻繩早已腐爛。遠遠看得見一間小木屋的屋頂探出樹梢，屋簷下垂著一串風乾的草藥，屋前的泥地掃得乾乾淨淨。
LONG
    );
    set("exits", ([
        "north" : __DIR__"cabin",
        "south" : __DIR__"dirt_road2",
    ]));
    set("map/area", "天山");
    setup();
}
