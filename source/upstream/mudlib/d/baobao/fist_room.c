// /d/baobao/fist_room.c — 八寶樓（F8）。敘述：舊資料（全文）。設計見 docs/AREA_BAOBAO.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "拳屋");
    set("long", @LONG
這裡的擺設與普通的民房沒什麼兩樣。只是牆上挂著的一張人體穴位圖和一張骨骼結構圖使人內心深處隱約感到一絲不安。
LONG
    );
    set("exits", ([
        "up" : __DIR__"ghost_realm",
    ]));
    set("objects", ([
        "/custom/legacy_npc/ma_zhong_shen" : 1,
    ]));
    set("map/area", "八寶樓");
    set("map/mode", "maze");
    setup();
    replace_program(ROOM);
}
