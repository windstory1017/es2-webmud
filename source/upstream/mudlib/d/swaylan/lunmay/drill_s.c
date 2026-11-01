// /d/swaylan/lunmay/drill_s.c — 冷梅莊（K25）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "練武場");
    set("long", @LONG
練武場的南半邊立著幾具木頭假人，身上佈滿了劍痕，有些地方已被刺穿了好幾個窟窿。這裡是入門弟子打根基之處，石板上滴滿了汗水留下的深色斑點，一旁的水缸裡浮著一隻葫蘆瓢。
LONG
    );
    set("exits", ([
        "north" : __DIR__"drill_n",
        "west" : __DIR__"front_yard",
    ]));
    set("objects", ([
        __DIR__"npc/stake" : 2,
        __DIR__"npc/apprentice3" : 1,
    ]));
    set("map/area", "冷梅莊");
    setup();
    replace_program(ROOM);
}
