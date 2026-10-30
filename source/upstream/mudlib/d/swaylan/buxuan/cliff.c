// /d/swaylan/buxuan/cliff.c — 落雲峰（AO12）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_climb", "climb");
}

/* 爬山壁（設計表）：基本閃躲 60 或機敏 25 才爬得上去。 */
int do_climb(string arg)
{
    object me = this_player();

    if( arg != "山壁" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    if( me->query_skill("dodge", 1) < 60 && me->query_attr("dex") < 25 )
        return notify_fail("山壁陡峭異常，你看了看腳下的深谷，終究不敢輕舉妄動。\n");
    message_vision("$N踩著石壁上的落腳點，手腳並用，幾個起落便攀上了峭壁。\n", me);
    if( !me->move("/d/swaylan/buxuan/cliff_top") ) return 0;
    message("vision", replace_string("$N從峭壁下攀了上來。", "$N", me->name()) + "\n", environment(me), ({ me }));
    return 1;
}

void create()
{
    set("short", "山壁");
    set("long", @LONG
一面陡峭山壁直插入雲，岩石灰黑，表面被風霜蝕得坑坑窪窪。仔細看去，石壁上有幾處明顯的落腳點，邊緣被磨得光滑，還留著鞋底的泥印，看來時常有人從這裡攀上攀下。
LONG
    );
    set("exits", ([
        "east" : __DIR__"peak_foot",
        "south" : __DIR__"terrace",
    ]));
    set("map/area", "水嵐縣");
    setup();
}
