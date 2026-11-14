// /d/oldpine/cave_deep.c — 迷霧森林（AD14）。房間敘述依設計表提示撰寫。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_climb", "climb");
}

int do_climb(string arg)
{
    object me = this_player();

    if( arg != "石頭" ) return notify_fail("你要爬什麼？\n");
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N手腳並用，攀上了洞角的大石頭。\n", me);
    if( !me->move("/d/oldpine/ledge") ) return 0;
    message("vision", replace_string("$N從下面爬了上來，喘了一口大氣。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void create()
{
    set("short", "山洞深處");
    set("long", @LONG
山洞的最深處，空間突然開闊起來，洞頂高得看不見，四周的岩壁上掛著水珠，在黑暗中閃著微光。地上散落著大量獸骨，還有幾件破爛的衣物和一只生鏽的鐵鍋，不知是哪個倒楣的獵人留下的。洞的一角有一塊巨大的石頭，石面上有幾處被踩出來的凹痕，看起來可以攀爬上去。
LONG
    );
    set("detail/石頭", "這塊大石頭幾乎頂到洞頂，石面上有幾處被踩出來的凹痕，手腳並用的話，可以爬 (climb) 上去。");
    set("exits", ([
        "north" : __DIR__"cave2",
        "west" : __DIR__"cave3",
    ]));
    set("objects", ([
        __DIR__"npc/little_bear" : 2,
        __DIR__"npc/big_bear" : 1,
    ]));
    set("map/area", "迷霧森林");
    setup();
}
