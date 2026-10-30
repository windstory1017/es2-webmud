// /d/ziyan/ferry.c — 紫煙鎮（G4）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "渡口");
    set("long", @LONG
紫煙鎮的渡口緊挨著羿水，幾根木樁歪歪斜斜地插在泥灘上，繫著一兩條破舊的小船。河面寬闊，對岸的五堂鎮隱約可見，岸邊卻空空蕩蕩，只有小船在水裡輕輕搖晃，船板吱呀作響。
LONG
    );
    set("exits", ([
        "south" : __DIR__"dirt_road",
    ]));
    set("no_fight", 1);
    set("map/area", "紫煙鎮");
    setup();
    replace_program(ROOM);
}
