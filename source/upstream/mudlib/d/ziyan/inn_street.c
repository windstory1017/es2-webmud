// /d/ziyan/inn_street.c — 紫煙鎮（O14）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "客棧街");
    set("long", @LONG
街道到這裡轉了個彎，旁邊是一間頗具規模的客棧，門口的酒旗洗得發白，倒還算醒目。這一帶比鎮裡熱鬧些，過往的旅人多半在此歇腳，趕車的、挑擔的來來往往，揚起一路塵土。
LONG
    );
    set("exits", ([
        "north" : __DIR__"market",
        "south" : __DIR__"entrance",
        "west" : __DIR__"inn_gate",
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
