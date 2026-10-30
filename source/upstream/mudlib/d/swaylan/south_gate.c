// /d/swaylan/south_gate.c — 水嵐縣（U24）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "南城門");
    set("long", @LONG
水嵐縣的南城門，城門高大，門洞裡涼颼颼的，幾名守衛手按兵器，冷眼打量著進出的行人。出城的官道筆直地伸向西南，是往京畿去的大路，商旅車馬絡繹不絕，揚起一路黃塵。
LONG
    );
    set("exits", ([
        "north" : __DIR__"south_street3",
    ]));
    set("objects", ([
        __DIR__"npc/gate_guard" : 4,
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
