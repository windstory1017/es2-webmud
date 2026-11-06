// /d/snowkeep/hall.c — 雪吟莊（J8）。依使用者提供的雪吟莊設計表（2026-11-06）；敘述：舊資料（全文引用）。
// 區域文件見 docs/AREA_SNOWKEEP.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "雪吟莊大廳");
    set("long", @LONG
這裡是雪吟莊的大廳﹐大廳前方原來有一條莊主引以為傲的大虎毛皮﹐但是為了表示對武林盟主於蘭兼毅的尊敬﹐所以換成了一幅京城才子張于辰的字畫﹐大廳的南北兩頭各有迴廊通往莊子內院﹐往西則是通往庭園的大門。
LONG
    );
    set("exits", ([
        "west" : __DIR__"yard_e",
        "east" : __DIR__"inner_hall",
        "north" : __DIR__"side_n",
        "south" : __DIR__"side_s",
    ]));
    set("objects", ([
        "/custom/legacy_npc/chu_xang_when" : 1,
    ]));
    set("map/area", "雪吟莊");
    setup();
    replace_program(ROOM);
}
