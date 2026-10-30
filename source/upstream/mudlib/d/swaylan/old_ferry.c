// /d/swaylan/old_ferry.c — 水嵐縣（W10）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "舊渡口");
    set("long", @LONG
這裡原是水嵐縣的渡口，自從河道淤積，船家都搬去了新渡口，碼頭便荒廢下來。半截石階斜斜伸進淤泥裡，長滿了蘆葦，四周的圍牆坍出好幾處破洞，只剩幾隻水鳥在泥灘上踱來踱去。
LONG
    );
    set("exits", ([
        "south" : __DIR__"dirt_road",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
