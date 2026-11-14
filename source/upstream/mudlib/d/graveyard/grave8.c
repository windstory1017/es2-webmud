// /d/graveyard/grave8.c — 亂葬崗（D6）。房間敘述舊資料全文引用。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_pass", "pass");
}

int do_pass(string arg)
{
    object me = this_player();

    if( arg != "草叢" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N撥開茂密的草叢，鑽了進去。\n", me);
    if( !me->move(__DIR__"weeds") ) return 0;
    message("vision", replace_string("$N撥開草叢，從另一頭鑽了出來。", "$N", me->name()) + "\n", environment(me), ({ me }));
    return 1;
}

void create()
{
    set("short", "亂葬崗");
    set("long", @LONG
這裡是亂葬崗中的一條小路﹐你的東邊是一個破敗的石亭﹐小路往
東北與西邊延伸﹐南邊是一片茂密的草叢﹐草叢中突起一堆堆的土墳﹐
草叢南邊是一片密林。
LONG
    );
    set("outdoors", "snow");
    set("detail/草叢", "南邊的草叢密不透風，草間隱約有一道被人鑽出來的縫隙，似乎可以穿過 (pass) 去。");
    set("exits", ([
        "east" : __DIR__"grave7",
        "west" : "/d/taiyi/gate",   // 太乙觀門口（使用者定案，2026-11-09）
    ]));
    set("map/area", "亂葬崗");
    setup();
}
