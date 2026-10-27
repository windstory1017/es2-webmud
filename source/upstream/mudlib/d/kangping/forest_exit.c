// /d/kangping/forest_exit.c — 康平村（D6）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "樹林口");
    set("long", @LONG
林木在這裡漸漸稀疏，陽光穿過枝葉灑落一地，腳下的落葉也少了許多。遠處山坳間升起幾縷炊煙，隱約可見一座小村落依偎在三煙谷口，看來總算要走出這片樹林了。
LONG
    );
    set("exits", ([
        "west" : "/d/oldpine/grass1",
        "east" : __DIR__"village_gate",
    ]));
    set("outdoors", "kangping");
    set("map/area", "康平村");
    setup();
    replace_program(ROOM);
}
