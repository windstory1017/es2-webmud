// /d/swaylan/escort_side.c — 水嵐縣（Q18）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "側廳");
    set("long", @LONG
一間不大的側廳，靠牆擺著幾張長桌，桌上堆滿了帳冊與鏢單，一旁的木架上掛著各路鏢旗。這裡是鏢頭們分派鏢務的地方，空氣中有墨汁與舊紙的氣味，看來平日裡往來調度甚是繁忙。
LONG
    );
    set("exits", ([
        "south" : __DIR__"escort_hall",
        "west" : __DIR__"escort_yard",
    ]));
    set("objects", ([
        "/custom/legacy_npc/kao_fon" : 1,
    ]));
    set("map/area", "黃家鏢局");
    setup();
    replace_program(ROOM);
}
