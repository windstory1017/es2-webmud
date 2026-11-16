// /d/qiaoyin_east/court.c — 白雲觀（F4）。依使用者設計表（2026-11-16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "中庭");
    set("long", @LONG
中庭鋪著整片的青石板，正中一座三足銅鼎，鼎裏的香灰積得老高，檀香的氣息淡淡地縈繞不散。四面廊廡環抱，簷下的朱漆已褪成暗紅，石板縫裏鑽出幾莖青苔，有種不為歲月所擾的清幽。
LONG
    );
    set("exits", ([
        "north" : __DIR__"yard",
        "south" : __DIR__"hall",
        "west" : __DIR__"corridor_w",
        "east" : __DIR__"corridor_e",
    ]));
    set("map/area", "白雲觀");
    setup();
    replace_program(ROOM);
}
