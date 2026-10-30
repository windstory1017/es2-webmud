// /d/wutang/ferry_boat_huang.c — 羿水渡船（小黃的船）：五堂鎮河邊 ↔ 紫煙鎮渡口的擺渡途中。
// 沒有出口，船夫靠岸時會把乘客帶上岸（d/wutang/npc/ferryman.c）。非戰區。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "渡船上");
    set("long", @LONG
一條新漆過的小舢舨，船頭貼著一張褪了色的紅紙，寫著「順風順水」四個字。小黃在船尾使勁搖著櫓，小船走得又快又急，濺起的水花不時打上船舷，水面的涼意一陣陣撲上臉來。
LONG
    );
    set("no_fight", 1);
    set("map/area", "羿水");
    set("map/layer", "小黃的船");   // 兩艘船各一層，地圖上只顯示自己這一艘
    setup();
    replace_program(ROOM);
}
