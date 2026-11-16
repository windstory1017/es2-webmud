// /d/qiaoyin_east/hall.c — 白雲觀（F6）。依使用者設計表（2026-11-16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "三清殿");
    set("long", @LONG
殿中供著三清塑像，神像面容祥和，衣紋上的金漆雖已剝落，卻仍看得出當年的華貴。供桌上的長明燈靜靜燃著，銅磬與木魚擺得齊整。殿門朝外敞開，香客與過路人進出，也不見有人阻攔。
LONG
    );
    set("exits", ([
        "north" : __DIR__"court",
        "west" : __DIR__"sutra_hall",
        "east" : __DIR__"refectory",
        "out" : __DIR__"outside",
    ]));
    set("objects", ([
        __DIR__"npc/tianfong" : 1,
    ]));
    set("map/area", "白雲觀");
    setup();
    replace_program(ROOM);
}
