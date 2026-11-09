// /d/taiyi/cliff.c — 山崖邊（P6）。房間敘述依設計表需求撰寫（docs/AREA_TAIYI.md）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "山崖邊");
    set("long", @LONG
通道盡頭是一處突出山壁的崖台，腳下山勢陡落，雪亭鎮的屋舍縮成了小小一片，溪流如銀線蜿蜒而過，遠處青邪大山層層疊疊，直入天際。獨立崖邊，衣袂獵獵，頗有獨上高樓，望盡天涯路之感。
LONG
    );
    set("outdoors", "snow");
    set("exits", ([
        "west" : __DIR__"passage3",
    ]));
    set("objects", ([
        __DIR__"npc/hsuan_ching" : 1,
    ]));
    set("map/area", "太乙觀");
    setup();
    replace_program(ROOM);
}
