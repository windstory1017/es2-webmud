// /d/swaylan/street1.c — 水嵐縣（W20）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "街道");
    set("long", @LONG
一條尋常的街道，兩旁是賣豆腐、醬菜的小鋪子，鋪面不大，卻收拾得乾乾淨淨。醬缸的酸香混著豆漿的甜味在街上飄散，來往的多是附近的街坊，彼此熟識，見面總要寒暄兩句。
LONG
    );
    set("exits", ([
        "west" : __DIR__"south_street2",
        "east" : __DIR__"street2",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
