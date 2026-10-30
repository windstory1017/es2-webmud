// /d/ziyan/well.c — 紫煙鎮（M8）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "井邊");
    set("long", @LONG
這一帶住著不少靠渡口討生活的人家，屋簷下掛著曬乾的魚，空氣裡帶著淡淡的魚腥味。街心有一口老井，幾個婦人圍在井邊洗衣，壓低了聲音說著鎮上的兇案，見有生人走近，便都住了口。
LONG
    );
    set("exits", ([
        "east" : __DIR__"corner",
        "west" : __DIR__"street",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
