// /d/mengjia/street.c — 孟家莊（F12）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "大街");
    set("long", @LONG
大街不長，兩旁的店舖多半只做本鄉人的生意，街口幾株老槐撐開一大片濃蔭，樹下總有幾位老人家搖著蒲扇閒話家常。街的盡頭古松漸密，濃濃的松脂味裡，隱約夾雜著一絲說不上來的陰冷。
LONG
    );
    set("exits", ([
        "west" : __DIR__"crossroad",
        "north" : __DIR__"forest",
    ]));
    set("objects", ([
        __DIR__"npc/meng_zhao_zhan" : 1,
    ]));
    set("map/area", "孟家莊");
    setup();
    replace_program(ROOM);
}
