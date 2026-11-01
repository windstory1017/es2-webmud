// /d/swaylan/lunmay/pavilion.c — 冷梅莊（G23）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "演武亭");
    set("long", @LONG
一座六角飛簷小亭，亭中擺著石桌石凳，是莊中長輩觀看弟子練劍的地方。亭柱上刻著一副對聯，字跡清瘦如梅枝，亭外幾株老梅環繞，坐在這裡，練武場上的劍光身影一覽無遺。
LONG
    );
    set("exits", ([
        "east" : __DIR__"courtyard",
    ]));
    set("map/area", "冷梅莊");
    setup();
    replace_program(ROOM);
}
