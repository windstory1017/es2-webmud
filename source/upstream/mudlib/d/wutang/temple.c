// /d/wutang/temple.c — 五堂鎮（X4）。房間敘述照設計表原文。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_enter", "enter");
}

int do_enter(string arg)
{
    object me = this_player();

    if( arg != "後院" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N穿過廟旁的小路，往後院走去。\n", me);
    if( !me->move("/d/wutang/temple_yard") ) return 0;
    message("vision", replace_string("$N從廟旁的小路走了過來。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

// 房間重置（reborn）時，廟祝補上一把飲血（蝶影幻步任務，每次重置只有一把）。
void reset()
{
    object npc;

    ::reset();
    if( objectp(npc = present("keeper", this_object())) ) npc->reset_stock();
}

void create()
{
    set("short", "鎮天神廟");
    set("long", @LONG
這裡是這個地方最有名的鎮天神廟，也是這地方最古老的廟，從這間廟建築的老舊可是卻香火鼎盛，就可以知道人們一定認為這間廟很靈，而這間廟主要是供奉著雲棧鎮天神，在廟前的廣場通常也是村人舉行活動的好地方，而旁邊也有一條進入後院的路。
LONG
    );
    set("exits", ([
        "south" : __DIR__"temple_road_n",
    ]));
    set("objects", ([
        __DIR__"npc/keeper" : 1,
    ]));
    set("map/area", "五堂鎮");
    setup();
}
