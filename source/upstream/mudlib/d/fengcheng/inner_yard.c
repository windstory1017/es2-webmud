// /d/fengcheng/inner_yard.c — 風城（H12）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "內院廣場");
    set("long", @LONG
將軍府的內院廣場鋪著平整的青石，四周樓閣環立，飛簷相接，其中最為高大的一座便是傳聞中的八寶樓。聽說府裡養著不少能人異士，此刻院中卻安靜得很，只有石縫間的風聲來回打轉。
LONG
    );
    set("exits", ([
        "south" : __DIR__"general_gate",
    ]));
    set("map/area", "風城");
    setup();
    replace_program(ROOM);
}
