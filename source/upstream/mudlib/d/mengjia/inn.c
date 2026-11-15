// /d/mengjia/inn.c — 孟家莊（D10）。

#include <room.h>

inherit INN;

void create()
{
    set("short", "小客棧");
    set("long", @LONG
小客棧只有兩層，大堂收拾得乾乾淨淨，幾張方桌擺得整整齊齊，過路的商旅與江湖客三三兩兩坐著，低聲談著北邊的見聞。梯口堆著幾個酒罈，木梯被腳步磨得發亮，樓上隱約傳來走動的聲響。
LONG
    );
    set("exits", ([
        "south" : __DIR__"crossroad",
        "up" : __DIR__"inn2",
    ]));
    set("map/area", "孟家莊");
    setup();
}
