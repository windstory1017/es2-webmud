// /d/wutang/bank.c — 五堂鎮（P14）。房間敘述依設計表需求擴寫。

#include <room.h>

inherit BANK;

void create()
{
    set("short", "景隆錢莊");
    set("long", @LONG
五堂鎮最大的景隆錢莊，櫃臺後的夥計正劈哩啪啦地打著算盤，牆上掛著一塊「信義通商」的金字匾額。幾名身材魁梧的護院武師守在門口與內堂，讓人不敢有非分之想。在這裡可以存款、提款，也可以兌換銀兩。
LONG
    );
    set("exits", ([
        "north" : __DIR__"east_street1",
    ]));
    set("objects", ([
        __DIR__"npc/guard" : 4,
    ]));
    set("map/area", "五堂鎮");
    setup();
}
