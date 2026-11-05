// /d/graveyard/grave1.c — 亂葬崗（P6）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "亂葬崗");
    set("long", @LONG
從雪亭鎮西邊的官道岔進來，人聲便漸漸遠了，眼前是一片荒涼的坡地，雜草間錯落著大大小小的土墳。一條被人踩出來的小徑彎彎曲曲地往坡上爬去，路口一塊歪斜的石碑上，刻著的字早已模糊難辨。
LONG
    );
    set("outdoors", "snow");
    set("exits", ([
        "west" : __DIR__"grave2",
        "southeast" : "/d/snow/wgate",
    ]));
    set("map/area", "亂葬崗");
    setup();
    replace_program(ROOM);
}
