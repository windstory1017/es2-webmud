// /d/graveyard/forest.c — 亂葬崗（D10）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "密林");
    set("long", @LONG
林中古木參天，枝葉交纏成一片濃密的頂蓋，連天光也透不下來，四下裏一片昏暗。腳下的腐葉積得極厚，一踩便陷下去半寸，冒出一股霉爛的氣味。樹幹上爬滿了濕滑的青苔，林子深處偶爾傳來枯枝折斷的聲響。
LONG
    );
    set("outdoors", "snow");
    set("exits", ([
        "north" : __DIR__"weeds",
        "west" : __DIR__"forest_w",
        "east" : __DIR__"forest_e",
    ]));
    set("map/area", "亂葬崗");
    setup();
    replace_program(ROOM);
}
