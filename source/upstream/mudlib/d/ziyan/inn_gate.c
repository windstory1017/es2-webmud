// /d/ziyan/inn_gate.c — 紫煙鎮（M14）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "紫煙小棧門口");
    set("long", @LONG
紫煙小棧門口，兩根木柱撐著一塊簡陋的招牌，招牌上的字被煙燻得發黑。門檻邊放著一只破碗，裡頭剩著些殘羹，引得一條野狗在附近徘徊不去，不時抬頭嗅嗅從店裡飄出的肉香。
LONG
    );
    set("exits", ([
        "east" : __DIR__"inn_street",
        "west" : __DIR__"inn",
    ]));
    set("objects", ([
        "/d/qiaoyin/npc/wild_dog" : 1,
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
