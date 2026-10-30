// /d/swaylan/buxuan/terrace.c — 步玄派（AO14）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "平台");
    set("long", @LONG
石階半途的一片平台，地面用石板鋪得平整，四周圍著一道低矮的石欄。倚欄望去，山下的田畝與池塘盡收眼底，再遠便是一片茫茫雲海，雲氣翻湧，教人胸襟為之一闊。
LONG
    );
    set("exits", ([
        "west" : __DIR__"steps1",
        "east" : __DIR__"steps2",
        "north" : __DIR__"cliff",
        "south" : __DIR__"field1",
    ]));
    set("map/area", "落雲峰");
    setup();
    replace_program(ROOM);
}
