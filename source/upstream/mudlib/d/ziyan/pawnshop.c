// /d/ziyan/pawnshop.c — 紫煙鎮（O6）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit HOCKSHOP;

void create()
{
    set("short", "當鋪");
    set("long", @LONG
紫煙鎮唯一的一間當鋪，門面窄小，高高的櫃檯後頭只開了一個小窗口，窗口上釘著粗鐵條。來這裡的多是走投無路的街坊，當的盡是些舊衣破鍋，櫃上的算盤卻打得一點也不含糊。
LONG
    );
    set("exits", ([
        "south" : __DIR__"corner",
    ]));
    set("no_fight", 1);
    set("map/area", "紫煙鎮");
    setup();
}
