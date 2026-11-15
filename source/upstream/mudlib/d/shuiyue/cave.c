// /d/shuiyue/cave.c — 水月村（H20）。

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

    if( arg != "縫隙" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N側著身子，從石壁的縫隙中縮身鑽了進去。\n", me);
    if( !me->move("/d/shuiyue/stone_house") ) return 0;
    message("vision", replace_string("$N從石壁的縫隙中縮身鑽了出來。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void create()
{
    set("short", "神秘山洞");
    set("long", @LONG
想不到瀑布的後面竟然有這樣一個山洞，山洞內只有微弱的陽光而且異常的潮濕。山洞四週的石壁上長滿了厚厚的青苔，看起來，這個山洞已經很久沒有人來過了。
LONG
    );
    set("detail/石壁", "石壁上有一道縫隙，寬度恰好可讓一人縮身進入 (enter)。");
    set("detail/縫隙", "這道縫隙窄得很，寬度恰好可讓一人縮身進入 (enter)。");
    set("exits", ([
        "out" : __DIR__"waterfall",
    ]));
    set("map/area", "水月村");
    set("map/layer", "洞穴");
    setup();
}
