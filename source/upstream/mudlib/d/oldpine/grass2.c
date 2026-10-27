// /d/oldpine/grass2.c — 老松林（V10）。房間敘述依設計表提示撰寫。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_cave", "cave");
}

int do_cave(string arg)
{
    object me = this_player();

    if( me->is_busy() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N撥開長草，彎身鑽進了山壁下的洞穴。\n", me);
    if( !me->move("/d/oldpine/bear_cave") ) return 0;
    message("vision", replace_string("$N從洞口彎身鑽了進來。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void create()
{
    set("short", "草叢");
    set("long", @LONG
這裡的野草長得比人還高，四周只聽得見草葉摩擦的沙沙聲，根本看不清三步以外的東西。草叢中被壓出了一條條獸徑，泥地上滿是爪印，草根處還散落著幾撮灰黃色的獸毛。北邊的山壁下有一個黑黝黝的洞穴，裡頭不時傳出低沉的吼聲。往西北可以回到較矮的草叢。
LONG
    );
    set("exits", ([
        "northwest" : __DIR__"grass1",
        "southeast" : "/d/baihua/hillside",   // 百花村（2026-10-20）
    ]));
    set("objects", ([
        __DIR__"npc/goat" : 3,   // 原為土狼（使用者指定改山羊，2026-10-24）
    ]));
    set("map/area", "老松林");
    setup();
}
