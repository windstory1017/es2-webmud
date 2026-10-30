// /d/swaylan/temple/side_hall.c — 寶蓮寺（AC12）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "側殿");
    set("long", @LONG
正殿旁的一間偏殿，殿中供著幾尊小小的菩薩像，案前的功德箱漆色斑駁，投錢的縫口被摸得發亮。信眾在這裡點燈添油，佛前一排長明燈火光搖曳，燈油的氣味混著檀香，在殿裡靜靜瀰漫。
LONG
    );
    set("exits", ([
        "southwest" : __DIR__"gate",
        "north" : __DIR__"main_hall",
    ]));
    set("map/area", "寶蓮寺");
    setup();
    replace_program(ROOM);
}
