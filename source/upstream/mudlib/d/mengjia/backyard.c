// /d/mengjia/backyard.c — 孟家莊（B8）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "後院");
    set("long", @LONG
後院不大，牆邊種著幾株石榴與一架藤蘿，石桌石凳擦得乾乾淨淨，像是常有人在此靜坐。牆角的兵器架上刀鞘排得整整齊齊，一眼望去便知這戶人家雖不問江湖事，卻從不曾荒廢家傳的武功。
LONG
    );
    set("exits", ([
        "south" : __DIR__"manor",
    ]));
    set("objects", ([
        __DIR__"npc/meng_yi_rou" : 1,
    ]));
    set("map/area", "孟家莊");
    setup();
    replace_program(ROOM);
}
