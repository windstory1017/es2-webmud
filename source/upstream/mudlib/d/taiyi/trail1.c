// /d/taiyi/trail1.c — 後山小路（J8）。房間敘述依設計表需求撰寫（docs/AREA_TAIYI.md）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "後山小路");
    set("long", @LONG
小路自觀後的石階蜿蜒而上，兩旁古松夾道，松針鋪了一地，踩上去軟軟的不出聲。回頭望去，太乙觀的飛簷隱在松枝之間，鐘磬聲也漸漸淡了，只餘山風一陣一陣拂過林梢。
LONG
    );
    set("outdoors", "snow");
    set("exits", ([
        "southwest" : __DIR__"corridor3",
        "north" : __DIR__"trail2",
    ]));
    set("map/area", "太乙觀");
    setup();
    replace_program(ROOM);
}
