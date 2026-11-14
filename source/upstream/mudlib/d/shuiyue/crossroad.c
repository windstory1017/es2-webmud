// /d/shuiyue/crossroad.c — 水月村（D28）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "十字路口");
    set("long", @LONG
村子的中心，四條大路在此交會，路口擺著一只大茶水桶，桶旁懸著一支竹勺，桶裡裝滿了清水，來往行人無論相識與否，都會停下來舀上一勺解渴。西邊那戶人家門前掃得乾乾淨淨，東邊客棧則不時傳出人聲與碗盤相碰的脆響。
LONG
    );
    set("exits", ([
        "north" : __DIR__"village1",
        "south" : __DIR__"village2",
        "west" : __DIR__"chief_house",
        "east" : __DIR__"inn",
    ]));
    set("objects", ([
        __DIR__"obj/tea_pot" : 1,
    ]));
    set("map/area", "水月村");
    setup();
    replace_program(ROOM);
}
