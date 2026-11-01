// /d/wutang/ferry.c — 五堂鎮（H18）。房間敘述依設計表需求擴寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "鯉君渡");
    set("long", @LONG
這裡是前往水嵐縣的鯉君渡，河岸邊停泊著幾艘渡船，船夫們吆喝著招攬過河的旅客。往北走下河堤，便是緊鄰河面的渡口，往東則是一條通往三岔路口的木道。
LONG
    );
    set("exits", ([
        "east" : __DIR__"boardwalk_w",
        "northdown" : __DIR__"ferry_dock",
    ]));
    set("no_fight", 1);   // 渡口一律非戰區（使用者指示，2026-10-30）
    set("map/area", "五堂鎮");
    setup();
    replace_program(ROOM);
}
