// /d/shuiyue/stone_road.c — 水月村（D22）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "石板路");
    set("long", @LONG
石板路被往來的腳步磨得溫潤發亮，縫隙裡冒出幾叢倔強的野草，路旁一條山溪淙淙流過，水聲清脆。遠遠有流水沖擊岩壁的悶響，從東邊的林子裡隱約傳來，挑擔的村民與背著包袱的遊客錯身而過，彼此點頭招呼，神情悠閒。
LONG
    );
    set("exits", ([
        "north" : "/d/fengcheng/gate",
        "south" : __DIR__"mud_road",
    ]));
    set("map/area", "水月村");
    setup();
    replace_program(ROOM);
}
