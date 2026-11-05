// /d/graveyard/grave5.c — 亂葬崗（H2）。房間敘述舊資料全文引用。

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

    if( arg != "小祠" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N撥開祠前的亂草，彎身走進了小祠。\n", me);
    if( !me->move(__DIR__"shrine") ) return 0;
    message("vision", replace_string("$N從外頭走了進來。", "$N", me->name()) + "\n", environment(me), ({ me }));
    return 1;
}

void create()
{
    set("short", "亂葬崗");
    set("long", @LONG
這裡是亂葬崗中的一條小路﹐小路上雜草叢生﹐北邊是一片亂石土
坡﹐土坡間突起一墳墳的無主孤墳﹐西邊一個土丘﹐土丘旁有一座小祠
﹐祭祀土丘中所葬的陣亡戰士﹐小路往西南與東邊延伸。
LONG
    );
    set("outdoors", "snow");
    set("exits", ([
        "southwest" : __DIR__"grave6",
        "east" : __DIR__"grave4",
    ]));
    set("objects", ([
        "/custom/legacy_npc/lady" : 1,
    ]));
    set("map/area", "亂葬崗");
    setup();
}
