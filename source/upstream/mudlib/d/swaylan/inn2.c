// /d/swaylan/inn2.c — 水嵐縣（S18）。房間敘述照設計表原文。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "五陵客棧二樓");
    set("long", @LONG
你現在在五陵客棧的二樓，這裡專供一些來往的客人休息用，不過通常只有有關係的人才能夠進來。在你的面前有幾張椅子，還有桌子，上面放一些茶水供人飲用。要是餓了，可以招呼店小二幫你送東西來。
LONG
    );
    set("exits", ([
        "down" : __DIR__"inn",
    ]));
    set("objects", ([
        __DIR__"npc/zhang_ling" : 1,
        __DIR__"npc/li_zai_yan" : 1,
    ]));
    set("no_fight", 1);
    set("map/area", "水嵐縣");
    set("map/layer", "二樓");
    setup();
    replace_program(ROOM);
}
