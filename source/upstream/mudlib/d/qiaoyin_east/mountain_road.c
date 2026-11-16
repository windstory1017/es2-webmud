// /d/qiaoyin_east/mountain_road.c — 喬陰東郊（H10）。依使用者設計表（2026-11-16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "山路");
    set("long", @LONG
山路隨著坡勢蜿蜒而上，路面夾碎石，被牛車與腳步碾出兩道深深的轍痕。路旁的雜草長得及膝，偶爾能見到折斷的箭桿與褪了色的布條，不知是哪一年留下的。松濤在山坳間來回盪著，讓這條路顯得格外空曠。
LONG
    );
    set("exits", ([
        "northwest" : __DIR__"outside",
        "eastdown" : __DIR__"fork",
    ]));
    set("map/area", "喬陰東郊");
    setup();
    replace_program(ROOM);
}
