// /d/swaylan/buxuan/slope.c — 落雲峰（AG18）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "山坡");
    set("long", @LONG
出城的小徑沿著山坡蜿蜒而上，坡上野草叢生，夾雜著幾株矮小的松樹，風過處沙沙作響。越往上走，水嵐縣城便越縮越小，回頭望去，城牆屋瓦都成了一片灰濛濛的影子。
LONG
    );
    set("exits", ([
        "northwest" : "/d/swaylan/hillside",
        "northeast" : __DIR__"mountain_path",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
