// /d/taiyi/passage3.c — 通道（N6）。房間敘述依設計表需求撰寫（docs/AREA_TAIYI.md）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "通道");
    set("long", @LONG
通道愈走愈寬，岩壁上留著斧鑿的痕跡，一盞早已熄滅的油燈掛在石釘上，燈罩積滿了灰。前方透進亮光，風聲也一陣緊似一陣，隱約夾著山下遠處傳來的人聲與犬吠。
LONG
    );
    set("exits", ([
        "west" : __DIR__"passage2",
        "east" : __DIR__"cliff",
    ]));
    set("map/layer", "地下");
    set("map/area", "太乙觀");
    setup();
    replace_program(ROOM);
}
