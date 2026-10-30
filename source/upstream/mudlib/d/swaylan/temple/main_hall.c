// /d/swaylan/temple/main_hall.c — 寶蓮寺（AC10）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "正殿");
    set("long", @LONG
寶蓮寺正殿高大軒敞，正中供著一尊白衣觀世音菩薩，手持淨瓶楊柳，低眉垂目，神情慈悲。殿內香煙繚繞，木魚聲篤篤不絕，幾個蒲團被跪得凹陷下去，看得出這裡的香火向來鼎盛。
LONG
    );
    set("exits", ([
        "south" : __DIR__"side_hall",
        "west" : __DIR__"dining",
        "east" : __DIR__"dorm",
    ]));
    set("no_fight", 1);
    set("map/area", "寶蓮寺");
    setup();
    replace_program(ROOM);
}
