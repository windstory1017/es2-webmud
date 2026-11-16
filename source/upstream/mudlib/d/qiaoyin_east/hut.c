// /d/qiaoyin_east/hut.c — 喬陰東郊（J14）。依使用者設計表（2026-11-16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "草廬");
    set("long", @LONG
草廬以竹為梁，茅草覆頂，屋裏擱著一張矮几與兩把竹椅，收拾得整齊。窗外種著大量的樹木，枝葉層層疊疊地貼著窗紗，把外頭的光篩得斑斑點點，屋內因此帶著幾分林間的幽靜。
LONG
    );
    set("exits", ([
        "north" : __DIR__"fork",
        "south" : __DIR__"clinic",
    ]));
    set("map/area", "喬陰東郊");
    setup();
    replace_program(ROOM);
}
