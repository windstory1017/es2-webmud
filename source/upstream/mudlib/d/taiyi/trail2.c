// /d/taiyi/trail2.c — 後山小路（J6）。房間敘述依設計表需求撰寫（docs/AREA_TAIYI.md）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "後山小路");
    set("long", @LONG
山路愈走愈陡，石階被經年的山水沖刷得坑坑窪窪，路旁的岩縫裏生著幾株倔強的野菊。林中寂靜，偶爾有鳥雀掠過枝頭，抬頭可見層層疊疊的山影，那便是青邪大山的外緣了。
LONG
    );
    set("outdoors", "snow");
    set("exits", ([
        "south" : __DIR__"trail1",
        "northwest" : __DIR__"trail3",
    ]));
    set("map/area", "太乙觀");
    setup();
    replace_program(ROOM);
}
