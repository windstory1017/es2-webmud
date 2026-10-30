// /d/swaylan/lunmay/hall.c — 冷梅莊（I21）。房間敘述照舊站「冷梅莊大廳」原文，出口一句依現況修改。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "冷梅莊正廳");
    set("long", @LONG
你來到了冷梅莊大廳裡，在大廳裡掛著一幅畫，畫中繪著許多梅樹。在畫的下方有一張太師椅，是莊主梅影風在處理事情時的座位，中央鋪著一塊地毯，上面繪著雪景。兩邊各有兩張座椅，是訪客來拜訪時坐的。向南穿過庭院可以通往前院，東西兩邊各有穿廊通往廳房。
LONG
    );
    set("exits", ([
        "west" : __DIR__"corridor_w",
        "east" : __DIR__"corridor_e",
        "south" : __DIR__"courtyard",
    ]));
    set("objects", ([
        "/d/wutang/npc/may_yin_fong" : 1,
        __DIR__"npc/goo" : 1,
    ]));
    set("map/area", "冷梅莊");
    setup();
    replace_program(ROOM);
}
