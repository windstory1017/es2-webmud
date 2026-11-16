// /d/qiaoyin_east/guest_room.c — 白雲觀（B6）。依使用者設計表（2026-11-16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "客房");
    set("long", @LONG
客房不大，一張木榻、一張方桌，被褥疊得整整齊齊，帶著曬過的乾草味。桌上的粗瓷茶壺與兩個茶杯擦得乾淨，想是為借宿的香客與過路人預備的。窗外有一枝松枝探進來，在窗欞上擦出細細的聲響。
LONG
    );
    set("exits", ([
        "east" : __DIR__"sutra_hall",
    ]));
    create_door("east", "房門", "west");
    set("map/area", "白雲觀");
    setup();
    replace_program(ROOM);
}
