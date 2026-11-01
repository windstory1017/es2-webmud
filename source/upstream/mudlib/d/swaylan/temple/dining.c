// /d/swaylan/temple/dining.c — 寶蓮寺（AA10）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "齋堂");
    set("long", @LONG
寺裡供香客用齋的地方，幾張長條木桌排得整整齊齊，桌上擺著粗瓷碗筷。灶間飄來米飯與青菜的清香，寶蓮寺向來為上香的信眾備一頓素齋，桌邊的香客低聲說話，吃得十分安靜。
LONG
    );
    set("exits", ([
        "east" : __DIR__"main_hall",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
