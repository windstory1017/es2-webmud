// /d/swaylan/alley.c — 水嵐縣（AA20）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "小巷子");
    set("long", @LONG
一條狹窄的小巷，兩旁高牆夾峙，只容得下兩人錯身而過。牆角長著濕滑的青苔，地上積著一灘灘汙水，透著一股霉味，與大街上的熱鬧比起來，這裡冷清得像是另一座城。
LONG
    );
    set("exits", ([
        "west" : __DIR__"street2",
        "northeast" : __DIR__"street_se",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
