// /d/baobao/forge.c — 八寶樓（D8）。敘述：舊資料（全文）。設計見 docs/AREA_BAOBAO.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "鑄刀房");
    set("long", @LONG
整個房間就好像是一個大熔爐，熱得讓人無法忍受。屋子中央支著一個大爐子，爐子裏不知在煉些什麼，爐子下面是旺盛的火焰，使人不敢靠近燒得發紅的火爐。
LONG
    );
    set("exits", ([
        "east" : __DIR__"fist_room",
    ]));
    set("objects", ([
        "/custom/legacy_npc/chong_bai_hu" : 1,
    ]));
    set("map/area", "八寶樓");
    set("map/mode", "maze");
    setup();
    replace_program(ROOM);
}
