// /d/taiyi/main_hall.c — 太乙觀正殿（L14）。房間敘述依設計表需求撰寫（docs/AREA_TAIYI.md）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "太乙觀正殿");
    set("long", @LONG
大殿高闊，梁柱漆得朱紅，殿心三尊神像並肩端坐，正是道門最尊的三清天尊，個個寶相莊嚴，垂眉俯視。供桌上香煙繚繞，蒲團前跪滿了祈福的信眾，木魚與誦經聲交織成一片，久久不散。
LONG
    );
    set("exits", ([
        "west" : __DIR__"hall",
        "east" : __DIR__"cell",
        "north" : __DIR__"yard",
    ]));
    set("objects", ([
        "/custom/legacy_npc/taoist_leader" : 1,
    ]));
    set("map/area", "太乙觀");
    setup();
    replace_program(ROOM);
}
