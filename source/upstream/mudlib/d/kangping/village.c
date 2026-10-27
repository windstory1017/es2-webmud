// /d/kangping/village.c — 康平村（F4）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "康平村");
    set("long", @LONG
康平村裡的一條土路，兩旁是幾戶低矮的農家，屋前曬著穀子與草藥，籬笆上爬滿了瓜藤。村民大多靠著上山採藥、打獵維生，見到生人也只是點點頭，又低頭忙著自己的活兒。
LONG
    );
    set("exits", ([
        "south" : __DIR__"village_gate",
        "east" : __DIR__"valley_mouth",
    ]));
    set("outdoors", "kangping");
    set("map/area", "康平村");
    setup();
    replace_program(ROOM);
}
