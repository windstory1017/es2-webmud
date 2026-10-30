// /d/wutang/ferry_boat_ro.c — 羿水渡船（老駱的船）：五堂鎮河邊 ↔ 紫煙鎮渡口的擺渡途中。
// 沒有出口，船夫靠岸時會把乘客帶上岸（d/wutang/npc/ferryman.c）。非戰區。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "渡船上");
    set("long", @LONG
一條老舊的烏篷小船，船身被河水浸得發黑，篷頂的竹篾修補過好幾回。老駱站在船尾，竹篙一點一撐，小船便穩穩地在羿水上滑行，兩岸的景物緩緩向後退去，只聽得見篙尖入水的輕響。
LONG
    );
    set("no_fight", 1);
    set("map/area", "羿水");
    setup();
    replace_program(ROOM);
}
