// /d/swaylan/hillside.c — 水嵐縣（AE16）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "山坡地");
    set("long", @LONG
出了東城門，地勢漸漸隆起，一片長滿芒草的山坡在眼前鋪開，幾條踩出來的小徑在草間交錯。遠處山巒層層疊疊，最高的一座峰頂終年雲霧繚繞，想必便是有名的落雲峰。
LONG
    );
    set("exits", ([
        "west" : __DIR__"east_gate",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
