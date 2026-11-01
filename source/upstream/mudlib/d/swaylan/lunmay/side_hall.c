// /d/swaylan/lunmay/side_hall.c — 冷梅莊（G19）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "側廳");
    set("long", @LONG
正廳西側的一間小廳，平日用來招待熟絡的客人，陳設比正廳隨意得多。窗下一張棋桌上還擺著半局殘棋，黑白子落得疏疏朗朗，一旁的茶盞早已涼透，看來下棋的人許久沒有回來了。
LONG
    );
    set("exits", ([
        "south" : __DIR__"corridor_w",
    ]));
    set("map/area", "冷梅莊");
    create_door("south", "木門", "north", 0);   // 平時開著，可以關上
    setup();
}
