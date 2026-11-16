// /d/qiaoyin_east/thicket.c — 喬陰東郊（H12）。依使用者設計表（2026-11-16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "雜草叢");
    set("long", @LONG
草叢裏比外頭看起來寬敞，雜草被壓出一小塊空地，地上留著幾處燒黑的灰燼與啃剩的獸骨，看來曾有人在此歇過腳。四周的草葉擠得密不透風，只剩來時那條被撥開的縫隙，還透進一絲亮來。
LONG
    );
    set("exits", ([
        "east" : __DIR__"fork",
    ]));
    set("map/area", "喬陰東郊");
    setup();
    replace_program(ROOM);
}
