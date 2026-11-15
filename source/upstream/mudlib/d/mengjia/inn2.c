// /d/mengjia/inn2.c — 孟家莊（D8）。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_push", "push");
}

int do_push(string arg)
{
    object me = this_player();

    if( arg != "床板" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N走到木床邊﹐用力推開鬆動的床板﹐翻身鑽了下去。\n", me);
    if( !me->move("/d/mengjia/passage") ) return 0;
    message("vision", replace_string("$N從上面的床板下鑽了下來。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void create()
{
    set("short", "客棧二樓");
    set("long", @LONG
二樓是一排簡陋的客房，木地板踩上去吱呀作響。最裡頭的一間房裡靠牆擺著一張舊木床，床板邊緣被磨得發白，像是常有人掀動，床底下隱約有一絲陰涼的風透上來。
LONG
    );
    set("detail/床板", "床板邊緣磨損得特別厲害﹐似乎常被人推動 (push)。");
    set("detail/木床", "一張舊木床﹐床板邊緣磨得發白﹐床底下有風透上來。");
    set("exits", ([
        "down" : __DIR__"inn",
    ]));
    set("objects", ([
        __DIR__"npc/tian_shen" : 1,
    ]));
    set("map/area", "孟家莊");
    set("map/layer", "二樓");
    setup();
}
