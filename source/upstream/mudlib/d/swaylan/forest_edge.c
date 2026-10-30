// /d/swaylan/forest_edge.c — 水嵐縣（AE14）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_enter", "enter");
}

/* 鑽進密林（第五批）：密林入口 out 回來。 */
int do_enter(string arg)
{
    object me = this_player();

    if( arg != "密林" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N撥開灌木叢的縫隙，彎腰鑽進了密林。\n", me);
    if( !me->move("/d/swaylan/forest/entrance") ) return 0;
    message("vision", replace_string("灌木叢一陣晃動，$N從外頭鑽了進來。", "$N", me->name()) + "\n", environment(me), ({ me }));
    return 1;
}

void create()
{
    set("short", "密林邊緣");
    set("long", @LONG
一片繁茂的密林在此展開，古木參天，枝葉交纏得密不透風，林中聽不見半聲鳥叫，靜得教人心裡發毛。林緣的灌木叢有一處被撥開了縫隙，斷枝上掛著幾縷破布，像是常有人從這裡鑽進鑽出。
LONG
    );
    set("exits", ([
        "southwest" : __DIR__"east_gate",
    ]));
    set("map/area", "水嵐縣");
    setup();
}
