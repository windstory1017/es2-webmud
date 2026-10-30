// /d/swaylan/west_gate.c — 水嵐縣（Q16）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "西城門");
    set("long", @LONG
水嵐縣的西城門，兩扇城門包著鐵皮，門釘已經生了鏽。出城的大路通往紫煙鎮，來往的多半是挑擔的小販與趕車的腳夫，城門口的石板被車輪碾得坑坑窪窪，積著一層薄薄的黃土。
LONG
    );
    set("exits", ([
        "north" : __DIR__"gambling_house",
        "east" : __DIR__"west_street",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
