// /d/swaylan/south_street1.c — 水嵐縣（U18）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "南大街");
    set("long", @LONG
南大街上多是住家，屋舍低矮整齊，門前曬著衣物和菜乾，巷口傳來孩童追逐嬉鬧的聲音。挑水的漢子從身邊走過，木桶一晃一晃，灑了一路的水跡，空氣中飄著誰家灶上燉肉的香味。
LONG
    );
    set("exits", ([
        "north" : __DIR__"inn",
        "south" : __DIR__"south_street2",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
