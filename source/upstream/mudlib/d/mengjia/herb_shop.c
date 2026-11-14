// /d/mengjia/herb_shop.c — 孟家莊（B14）。

#include <room.h>

inherit HERB;

void create()
{
    set("short", "同仁藥鋪");
    set("long", @LONG
這裡是一間藥鋪，如果你有什麼需要，可以向藥鋪的掌櫃訊問一下。這種山村裡的小藥鋪，雖然治不了什麼大病，但是治療一般的跌打損傷還是綽綽有餘。在你的正上方有一塊匾額，上面寫著濟世救人。
LONG
    );
    set("exits", ([
        "north" : __DIR__"gate",
    ]));
    set("objects", ([
        "/d/snow/npc/herbalist" : 1,
    ]));
    set("map/area", "孟家莊");
    setup();
}
