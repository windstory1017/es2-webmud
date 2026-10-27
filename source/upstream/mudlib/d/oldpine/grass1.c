// /d/oldpine/grass1.c — 老松林（T8）。房間敘述依設計表提示撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "草叢");
    set("long", @LONG
松林在這裡忽然斷開，眼前是一大片及腰的野草，風一吹便如波浪般起伏。草叢裡不時傳來窸窸窣窣的聲音，偶爾還能看見一雙發亮的眼睛在草間一閃而過，叫人心裡發毛。草地上有幾處被壓倒的痕跡，像是有什麼大傢伙在這裡打過滾。往西北可以回到松林，往北是一塊空地，往東南的草長得更高更密。
LONG
    );
    set("exits", ([
        "northwest" : __DIR__"forest_n3",
        "north" : __DIR__"clearing_n",
        "southeast" : __DIR__"grass2",
        "east" : "/d/kangping/forest_exit",   // 康平村（2026-10-20）
    ]));
    set("map/area", "老松林");
    setup();
    replace_program(ROOM);
}
