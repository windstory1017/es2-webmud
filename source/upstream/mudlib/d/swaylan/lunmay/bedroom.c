// /d/swaylan/lunmay/bedroom.c — 冷梅莊（K19）。房間敘述照設計表需求補寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "寢房");
    set("long", @LONG
房內陳設簡單，床榻旁擺著小几，上頭放著藥碗與幾帖尚未服用的藥方。屋裡瀰漫著一股苦澀的藥味，窗紙糊得嚴嚴實實，床帳低低垂著，帳後不時傳出幾聲壓抑的輕咳。
LONG
    );
    set("exits", ([
        "south" : __DIR__"corridor_e",
    ]));
    set("objects", ([
        __DIR__"npc/may_wife" : 1,
    ]));
    set("map/area", "冷梅莊");
    create_door("south", "木門", "north", 0);   // 平時開著，可以關上
    setup();
}
