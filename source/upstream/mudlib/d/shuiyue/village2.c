// /d/shuiyue/village2.c — 水月村（D30）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "水月村");
    set("long", @LONG
村子的南頭，幾戶人家的矮牆圍著小菜畦，竹籬笆上爬滿了牽牛花。遠處山腳下有一條蜿蜒向上的小路，緩緩隱入林間，偶爾有背著行囊的旅人從那邊走下來，在村裡討碗茶水再趕路。
LONG
    );
    set("exits", ([
        "north" : __DIR__"crossroad",
        "southdown" : __DIR__"mountain_path1",
    ]));
    set("map/area", "水月村");
    setup();
    replace_program(ROOM);
}
