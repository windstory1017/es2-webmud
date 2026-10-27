// /d/baihua/cliff.c — 百花村（H10）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void init()
{
    add_action("do_climb", "climb");
}

int do_climb(string arg)
{
    object me = this_player();

    if( arg != "峭壁" && arg != "cliff" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法分心攀爬。\n");
    message_vision("$N攀著峭壁上的岩縫與花藤，小心翼翼地往上爬去。\n", me);
    if( !me->move(__DIR__"platform") ) return 0;
    message("vision", me->name() + "從峭壁下方爬了上來。\n", environment(me), me);
    return 1;
}

void create()
{
    set("short", "百花崖");
    set("long", @LONG
走到這裏時你嚇了一跳, 原來瓦屋之後竟有這般寬闊的景色, 在你腳邊的是深不見底的懸崖. 你心中暗叫僥倖! 差一點就摔下去了. 原來這裏是百花村的風景名勝之一『百花崖』, 你往下看去, 各式各樣的花沿著峭壁生長, 或者這就是百花村的由來吧!
LONG
    );
    set("exits", ([
        "east" : __DIR__"tile_house",
    ]));
    set("objects", ([
        __DIR__"npc/dou_foo" : 1,
    ]));
    set("outdoors", "baihua");
    set("map/area", "百花村");
    setup();
}
