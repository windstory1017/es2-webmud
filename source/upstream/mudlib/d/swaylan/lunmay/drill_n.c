// /d/swaylan/lunmay/drill_n.c — 冷梅莊（K23）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "練武場");
    set("long", @LONG
一片寬闊的練武場，地上鋪著平整的青石板，被經年累月的步法踩出一圈圈淺痕。場邊的兵器架上插滿了長短不一的劍，寒光閃閃，弟子們正在對拆劍招，喝叱聲與劍刃交擊聲此起彼落。
LONG
    );
    set("exits", ([
        "south" : __DIR__"drill_s",
        "west" : __DIR__"courtyard",
    ]));
    set("objects", ([
        "/d/wutang/npc/apprentice" : 2,
    ]));
    set("map/area", "冷梅莊");
    setup();
    replace_program(ROOM);
}
