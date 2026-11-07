// /d/ziyan/drugstore.c — 紫煙鎮（E6）。房間敘述依設計表需求撰寫。

#include <room.h>

// 改為龍圖藥鋪櫃台：方士可 herblist／buy／sell 丹藥（2026-11-07）。
inherit HERB;

void create()
{
    set("short", "藥鋪");
    set("long", @LONG
一間不大的藥鋪，靠牆立著一排頂天的藥櫃，數不清的小抽屜上貼著泛黃的藥名籤。櫃檯上擺著戥子和碾槽，滿屋子都是草藥苦澀的氣味，後頭一扇小門半掩著，隱約可見院裡曬著藥材。
LONG
    );
    set("exits", ([
        "east" : __DIR__"dirt_road",
        "out" : __DIR__"herb_yard",
    ]));
    set("objects", ([
        __DIR__"npc/herbalist" : 1,
    ]));
    set("map/area", "水嵐縣");
    setup();
}
