// /d/snowkeep/garden_s.c — 雪吟莊（H6）。依使用者提供的雪吟莊設計表（2026-11-06）；敘述：新寫。
// 區域文件見 docs/AREA_SNOWKEEP.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "後花園");
    set("long", @LONG
花園裏假山錯落，一道清淺的溪水從石縫間流出，繞著假山蜿蜒而過，水聲淙淙。溪邊種著幾叢翠竹與芭蕉，水氣氤氳，幾隻蜻蜓在水面上輕輕點過，比起前院的人聲，這裡清幽得像是另一個天地。
LONG
    );
    set("exits", ([
        "north" : __DIR__"garden_n",
        "south" : __DIR__"yard_e",
    ]));
    set("map/area", "雪吟莊");
    setup();
    replace_program(ROOM);
}
