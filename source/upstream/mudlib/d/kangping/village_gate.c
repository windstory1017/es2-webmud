// /d/kangping/village_gate.c — 康平村（F6）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "康平村");
    set("long", @LONG
康平村的村口，一塊飽經風吹日曬的木牌插在路旁，上頭刻著「康平」兩個大字。這個小村落夾在樹林與三煙谷之間，平日少有外人往來，幾隻土雞在路邊悠閒地啄食，四下顯得格外安靜。
LONG
    );
    set("exits", ([
        "west" : __DIR__"forest_exit",
        "north" : __DIR__"village",
    ]));
    set("outdoors", "kangping");
    set("map/area", "康平村");
    setup();
    replace_program(ROOM);
}
