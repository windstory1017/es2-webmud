// /d/baihua/road1.c — 百花村（L14）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "常春道");
    set("long", @LONG
百花村的主要通道，路面鋪著平整的石板，兩旁的屋舍排列得整整齊齊。路上往來的人並不多，卻處處打掃得一塵不染，門前的花盆也照料得十分細心，看得出村民都很愛惜這個地方。
LONG
    );
    set("exits", ([
        "north" : __DIR__"stone_road2",
        "west" : __DIR__"trail",
        "east" : __DIR__"alley",
        "south" : __DIR__"road2",
    ]));
    set("outdoors", "baihua");
    set("map/area", "百花村");
    setup();
    replace_program(ROOM);
}
