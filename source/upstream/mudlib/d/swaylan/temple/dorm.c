// /d/swaylan/temple/dorm.c — 寶蓮寺（AE10）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "寮房");
    set("long", @LONG
一排簡樸的寮房，專供雲遊參學的行腳僧、道士借宿暫住，屋裡只有一張木板床和一盞油燈。寮房後頭隔著一道竹籬，是寺中女尼起居的地方，籬門緊閉，門上掛著一塊「閒人止步」的木牌。
LONG
    );
    set("exits", ([
        "west" : __DIR__"main_hall",
    ]));
    set("objects", ([
        __DIR__"npc/young_nun" : 1,
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
