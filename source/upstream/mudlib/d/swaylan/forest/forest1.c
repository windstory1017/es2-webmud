// /d/swaylan/forest/forest1.c — 密林（AE23）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "密林");
    set("long", @LONG
林子越走越深，腳下的落葉積了厚厚一層，踩上去軟綿綿的，散發著一股腐爛的氣味。四下靜得出奇，連鳥叫聲都聽不見，只有偶爾一陣窸窸窣窣的聲響，從看不見的暗處傳來，教人背脊發涼。
LONG
    );
    set("exits", ([
        "west" : __DIR__"entrance",
        "east" : __DIR__"forest2",
    ]));
    set("objects", ([
        __DIR__"npc/bandit" : 3,
    ]));
    set("map/area", "密林");
    setup();
    replace_program(ROOM);
}
