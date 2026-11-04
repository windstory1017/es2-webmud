// /d/graveyard/grave4.c — 亂葬崗（J2）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "亂葬崗");
    set("long", @LONG
小路在這一段被兩側的墳堆擠得越發窄了，有幾處墳頭塌了半邊，露出朽爛的棺木一角，教人不敢多看。路旁斜插著幾塊無字的木牌，早已被蟲蛀得千瘡百孔，空氣中隱隱飄著一股潮濕的土腥氣。
LONG
    );
    set("outdoors", "snow");
    set("exits", ([
        "west" : __DIR__"grave5",
        "southeast" : __DIR__"grave3",
    ]));
    set("map/area", "亂葬崗");
    setup();
    replace_program(ROOM);
}
