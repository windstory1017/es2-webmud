// /d/shuiyue/inn.c — 水月村（F28）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "福安客棧");
    set("long", @LONG
你走進一家規模頗大的客棧; 偌大的樓面坐滿了客人, 幾乎找不到空位入座. 酒菜的香氣從廚房中飄出, 令人聞之食指大動, 饞涎欲滴. 這家客棧專供外來遊客歇腳之用, 樓上還備有乾淨的客房讓外地人住宿, 在掌櫃的熱誠招待下, 許多遊人一再到此光顧, 因而建立起良好的聲譽、口碑.
LONG
    );
    set("exits", ([
        "west" : __DIR__"crossroad",
    ]));
    set("objects", ([
        __DIR__"npc/innkeeper" : 1,
    ]));
    set("no_fight", 1);   // 非戰區；不是旅館，不能存檔
    set("map/area", "水月村");
    setup();
    replace_program(ROOM);
}
