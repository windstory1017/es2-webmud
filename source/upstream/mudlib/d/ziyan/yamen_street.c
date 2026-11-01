// /d/ziyan/yamen_street.c — 紫煙鎮（G8）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "縣衙前街");
    set("long", @LONG
縣衙門前一段街道，街面倒是掃得乾乾淨淨，與鎮上別處的破落大不相同。牆上貼著幾張褪色的告示，催繳稅銀的字樣寫得斗大，行人經過時都低著頭，腳步匆匆，不敢多看一眼。
LONG
    );
    set("exits", ([
        "east" : __DIR__"narrow_street",
        "north" : __DIR__"dirt_road",
        "south" : __DIR__"yamen_gate",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
