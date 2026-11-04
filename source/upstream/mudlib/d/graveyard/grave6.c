// /d/graveyard/grave6.c — 亂葬崗（F4）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "亂葬崗");
    set("long", @LONG
這裡的墳頭比別處齊整些，幾座石碑上依稀還認得出「征奐」「陣亡」等字樣，想必與那座小祠所祭的將士同出一役。碑前積著厚厚的落葉，沒有半點香火的痕跡，荒草從碑縫裏鑽出來，幾乎要把碑文整個掩去。
LONG
    );
    set("outdoors", "snow");
    set("exits", ([
        "northeast" : __DIR__"grave5",
        "south" : __DIR__"grave7",
    ]));
    set("map/area", "亂葬崗");
    setup();
    replace_program(ROOM);
}
