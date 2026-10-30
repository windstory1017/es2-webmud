// /d/swaylan/east_street3.c — 水嵐縣（AA16）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "東大街");
    set("long", @LONG
街道到這裡漸漸靠近東城門，幾名守衛在街邊來回巡視，神色警覺。北邊岔出一條石板小路，盡頭是一座寺院的山門，檀香的氣味遠遠飄來，路口擺著賣香燭的攤子，上香的婦人絡繹不絕。
LONG
    );
    set("exits", ([
        "west" : __DIR__"east_street2",
        "east" : __DIR__"east_gate",
    ]));
    set("objects", ([
        __DIR__"npc/gate_guard" : 4,
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
