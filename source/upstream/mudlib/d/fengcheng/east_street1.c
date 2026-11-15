// /d/fengcheng/east_street1.c — 風城（J16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "東街");
    set("long", @LONG
越往東走，街面越見冷清，店舖漸漸稀疏，換成一戶戶帶院子的民宅，院牆上爬滿了藤蔓。偶有孩童的笑鬧聲從牆內傳出，與遠處山間的雲霧相映，透著幾分悠閒自在。
LONG
    );
    set("exits", ([
        "west" : __DIR__"street2",
        "east" : __DIR__"east_street2",
    ]));
    set("map/area", "風城");
    setup();
    replace_program(ROOM);
}
