// /d/swaylan/escort_hall.c — 水嵐縣（Q20）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "正廳");
    set("long", @LONG
鏢局的正廳寬敞明亮，正中懸著一塊「義氣千秋」的匾額，下方一張虎皮交椅，兩側各排著幾張太師椅。廳柱粗大，漆得朱紅，牆上掛著幾柄擦得雪亮的長刀，一股剛正之氣迎面而來。
LONG
    );
    set("exits", ([
        "east" : __DIR__"escort_gate",
        "north" : __DIR__"escort_side",
        "west" : __DIR__"escort_back",
    ]));
    set("objects", ([
        "/custom/legacy_npc/huang_tien_ba" : 1,
    ]));
    set("map/area", "黃家鏢局");
    setup();
    replace_program(ROOM);
}
