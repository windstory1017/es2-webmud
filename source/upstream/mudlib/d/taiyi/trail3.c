// /d/taiyi/trail3.c — 後山小路（H4）。房間敘述依設計表需求撰寫（docs/AREA_TAIYI.md）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "後山小路");
    set("long", @LONG
路到這裡已不成路，只剩一道被樵夫與獵戶踩出來的小徑，隱沒在茂密的灌木與藤蔓之間。山勢在前方陡然拔高，林影深處霧氣瀰漫，向來是這一帶終年不散的景象，青邪大山就在眼前。
LONG
    );
    set("outdoors", "snow");
    set("exits", ([
        "southeast" : __DIR__"trail2",
    ]));
    set("map/area", "太乙觀");
    setup();
    replace_program(ROOM);
}
