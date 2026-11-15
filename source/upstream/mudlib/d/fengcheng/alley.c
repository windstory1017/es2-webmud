// /d/fengcheng/alley.c — 風城（L18）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "小巷");
    set("long", @LONG
一條窄窄的小巷，兩旁高牆夾道，牆面斑駁，爬著零星的青苔。牆腳下被人用石灰畫滿了歪歪扭扭的字謎與記號，一看便是孩童的塗鴉，只有最裡頭的一面牆，擦得乾乾淨淨。
LONG
    );
    set("exits", ([
        "north" : __DIR__"east_street2",
    ]));
    set("objects", ([
        __DIR__"npc/child" : 1,
    ]));
    set("map/area", "風城");
    setup();
    replace_program(ROOM);
}
