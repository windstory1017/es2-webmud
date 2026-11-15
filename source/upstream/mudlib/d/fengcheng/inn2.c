// /d/fengcheng/inn2.c — 風城（F18 二樓）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "春風客棧二樓");
    set("long", @LONG
二樓是一排整齊的客房，木地板被來往的腳步磨得發亮，走廊盡頭一扇小窗透著光。每扇房門上都掛著小木牌，房裡偶爾傳出旅人的鼾聲，或是低低的談話聲，整層樓安靜而溫暖。
LONG
    );
    set("exits", ([
        "down" : __DIR__"inn",
    ]));
    set("objects", ([
        "/custom/legacy_npc/dong_fang_gu_ming" : 1,
    ]));
    set("map/area", "風城");
    set("map/layer", "二樓");
    setup();
    replace_program(ROOM);
}
