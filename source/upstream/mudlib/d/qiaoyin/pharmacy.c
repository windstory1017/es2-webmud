// /d/qiaoyin/pharmacy.c — 喬陰縣（R12）。依使用者設計表（2026-10-16）。

#include <room.h>

// 改為龍圖藥鋪櫃台：可查丹藥、買丹藥與配方、賣丹藥（使用者指示 #3，2026-10-23）。
inherit HERB;

void create()
{
    set("short", "同仁藥鋪");
    set("long", @LONG
這裡是一間藥鋪，如果你有什麼需要，可以向藥鋪的掌櫃訊問一下。這種山村裡的小藥鋪，雖然治不了什麼大病，但是治療一般的跌打損傷還是綽綽有餘的。在你的正上方有一塊匾額，上面寫著濟世救人。
LONG
    );
    set("exits", ([
        "north" : __DIR__"east_street3",
    ]));
    set("objects", ([
        __DIR__"npc/keeper" : 1,
    ]));
    set("map/area", "喬陰縣");
    setup();
}
