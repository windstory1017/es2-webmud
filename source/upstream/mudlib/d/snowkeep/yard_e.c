// /d/snowkeep/yard_e.c — 雪吟莊（H8）。依使用者提供的雪吟莊設計表（2026-11-06）；敘述：新寫。
// 區域文件見 docs/AREA_SNOWKEEP.md。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "前院");
    set("long", @LONG
前院到了這一頭漸漸寬闊，正面一座廳堂飛簷斗拱，簷下懸著一排紅燈籠，氣派非凡。院角一口大缸盛滿清水，缸邊擱著幾隻木瓢，前來赴會的賓客三三兩兩在此駐足，低聲談論著這次的英雄大會。
LONG
    );
    set("exits", ([
        "west" : __DIR__"yard_w",
        "east" : __DIR__"hall",
        "north" : __DIR__"garden_s",
    ]));
    set("objects", ([
        "/custom/legacy_npc/hsu_yen_chih" : 1,
        __DIR__"npc/wang_hsian" : 1,
    ]));
    set("map/area", "雪吟莊");
    setup();
    replace_program(ROOM);
}
