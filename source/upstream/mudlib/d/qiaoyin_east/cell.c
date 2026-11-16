// /d/qiaoyin_east/cell.c — 白雲觀（B4）。依使用者設計表（2026-11-16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "靜室");
    set("long", @LONG
這是一間狹小的靜室，地上放著一對蒲團，牆角的香爐裏插著三炷線香，煙氣筆直地往上升。窗紙糊得嚴嚴實實，外頭的聲響到了這裡都悶了下來，只剩自己的呼吸聲，壁上懸著一幅「靜」字條幅，墨色已有些褪淡。
LONG
    );
    set("exits", ([
        "east" : __DIR__"corridor_w",
    ]));
    create_door("east", "房門", "west", DOOR_CLOSED);
    set("map/area", "白雲觀");
    setup();
    replace_program(ROOM);
}
