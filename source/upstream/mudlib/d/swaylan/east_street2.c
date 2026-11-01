// /d/swaylan/east_street2.c — 水嵐縣（Y16）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "東大街");
    set("long", @LONG
這一段街面多是體面的鋪子，綢緞莊、香燭店一家挨著一家，招牌漆得金光閃閃。往來行人的衣著也講究了些，偶有轎子從街心抬過，轎夫腳步齊整，路人紛紛讓到兩旁。
LONG
    );
    set("exits", ([
        "west" : __DIR__"east_street1",
        "east" : __DIR__"east_street3",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
