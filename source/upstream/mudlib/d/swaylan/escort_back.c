// /d/swaylan/escort_back.c — 水嵐縣（O20）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "後廳");
    set("long", @LONG
後廳是總鏢頭招待貴客、商談要事的地方，陳設比前廳講究得多。紫檀木的桌椅上雕著雲紋，多寶格裡擺著青花瓷瓶與玉雕擺件，牆上掛著一幅山水古畫，案頭一爐沉香煙氣裊裊，雅致而不張揚。
LONG
    );
    set("exits", ([
        "north" : __DIR__"escort_yard",
        "east" : __DIR__"escort_hall",
    ]));
    set("map/area", "黃家鏢局");
    setup();
    replace_program(ROOM);
}
