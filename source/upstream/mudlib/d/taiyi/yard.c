// /d/taiyi/yard.c — 後院（L12）。房間敘述依設計表需求撰寫（docs/AREA_TAIYI.md）。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_climb", "climb");
}

/* 後院的古井：climb 繩索 往下爬到井底（敘述見 docs/AREA_TAIYI.md）。 */
int do_climb(string arg)
{
    object me = this_player();

    if( arg != "繩索" ) return notify_fail("這裡沒有什麼可以爬的。\n");
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    tell_object(me, "你抓住井口垂下的繩索，雙腳蹬著井壁，一步一步往下爬去。\n");
    message("vision", me->name() + "抓住井口的繩索，一步一步往井下爬去。\n", this_object(), ({ me }));
    if( !me->move(__DIR__"well") ) return 1;
    message("vision", me->name() + "順著繩索爬了下來。\n", environment(me), ({ me }));
    return 1;
}

void create()
{
    set("short", "後院");
    set("long", @LONG
後院不大，青磚縫裏長著幾叢野草，牆角堆著劈好的柴火，與前頭的熙攘比起來冷清了許多。院子一角有座不起眼的古井，井欄被磨得光滑，井口垂下一條繩索，也不知通向多深的地方。
LONG
    );
    set("outdoors", "snow");
    set("detail/繩索", "井口垂下的一條粗麻繩，不知通向多深的地方，抓住繩索可以往下爬 (climb) 到井裡去。");
    set("exits", ([
        "south" : __DIR__"main_hall",
    ]));
    set("map/area", "太乙觀");
    setup();
}
