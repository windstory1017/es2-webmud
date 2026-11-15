// /d/baobao/magic_realm.c — 八寶樓（F4）。敘述：舊資料（全文）。設計見 docs/AREA_BAOBAO.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "魔境");
    set("long", @LONG
這間屋子裏的書架上堆滿了有關法術咒術以及奇門五行的古書。讓人費解的是，書桌上所擺的施法用具似乎於天朝境內常見的用具不同。
LONG
    );
    set("exits", ([
        "west" : __DIR__"illusion_realm",
    ]));
    set("objects", ([
        "/custom/legacy_npc/bai_yu" : 1,
    ]));
    set("map/area", "八寶樓");
    set("map/mode", "maze");
    setup();
    replace_program(ROOM);
}
