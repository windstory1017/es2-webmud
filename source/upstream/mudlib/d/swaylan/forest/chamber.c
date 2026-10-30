// /d/swaylan/forest/chamber.c — 密林（AI29）。房間敘述照舊站「綠林大盜總部」原文（刪一字對齊）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "密室");
    set("long", @LONG
這裡就是綠林大盜總部﹐安排的相當隱密﹐一箱一箱的寶物和耀眼的兵器﹐掛在右上角的是名震江湖一時的鋼劍【流走】﹐左上角的是靜寂【破日】﹐正前方正是綠林大盜的總領賊帥﹐右眼罩裡又彷彿藏著暗器﹐一心慌想這真不是我該來的地方。
LONG
    );
    set("exits", ([
        "out" : __DIR__"hideout",
    ]));
    set("objects", ([
        __DIR__"npc/bandit_general" : 1,
    ]));
    set("map/area", "密林");
    set("map/layer", "地下");
    setup();
    replace_program(ROOM);
}
