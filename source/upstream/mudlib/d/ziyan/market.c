// /d/ziyan/market.c — 紫煙鎮（O12）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "菜市");
    set("long", @LONG
小街往南漸漸寬了些，路邊多了幾個賣菜賣魚的攤子，攤販有氣無力地吆喝著，買的人卻沒幾個。地上爛菜葉和魚鱗混在一起，被來往的腳步踩得稀爛，透著一股酸腐的氣味。
LONG
    );
    set("exits", ([
        "north" : __DIR__"lane",
        "south" : __DIR__"inn_street",
    ]));
    set("map/area", "紫煙鎮");
    setup();
    replace_program(ROOM);
}
