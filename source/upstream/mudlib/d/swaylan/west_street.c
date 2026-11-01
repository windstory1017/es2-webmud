// /d/swaylan/west_street.c — 水嵐縣（S16）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "西大街");
    set("long", @LONG
西大街兩旁多是賣雜貨、布疋的小店，門前掛著褪了色的幌子，夥計倚在門口招呼客人。街上行人來來往往，挑擔的、推車的擠成一團，叫賣聲與討價還價聲此起彼落，滿是市井氣息。
LONG
    );
    set("exits", ([
        "west" : __DIR__"west_gate",
        "east" : __DIR__"inn",
        "north" : __DIR__"bank",
    ]));
    set("objects", ([
        __DIR__"npc/man" : 1,
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
