// /d/qiaoyin_east/sutra_hall.c — 白雲觀（D6）。依使用者設計表（2026-11-16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "經堂");
    set("long", @LONG
堂中三面牆都是書架，擺滿了泛黃的經卷與道書，不少書脊被翻得起毛邊。靠窗的長案上放著筆墨與一本攤開的抄本，墨跡猶新，看來時常有人在此研讀。紙墨混著檀香的氣味，教人不自覺放輕了腳步。
LONG
    );
    set("exits", ([
        "north" : __DIR__"corridor_w",
        "west" : __DIR__"guest_room",
        "east" : __DIR__"hall",
    ]));
    create_door("west", "房門", "east", DOOR_CLOSED);
    set("objects", ([
        "/custom/legacy_npc/lay_ton" : 1,
    ]));
    set("map/area", "白雲觀");
    setup();
    replace_program(ROOM);
}
