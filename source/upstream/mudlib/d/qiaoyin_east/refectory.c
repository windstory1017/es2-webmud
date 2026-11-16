// /d/qiaoyin_east/refectory.c — 白雲觀（H6）。依使用者設計表（2026-11-16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "齋堂");
    set("long", @LONG
齋堂裏擺著幾張長條木桌，桌面被油水浸得發亮，牆角大灶上擱著一口燻黑的鐵鍋，米香淡淡地飄著。觀裏的道士們進進出出，碗筷碰撞的聲音此起彼落，在這避世的山觀裏顯出一份踏實的人間煙火。
LONG
    );
    set("exits", ([
        "north" : __DIR__"corridor_e",
        "west" : __DIR__"hall",
    ]));
    set("map/area", "白雲觀");
    setup();
    replace_program(ROOM);
}
