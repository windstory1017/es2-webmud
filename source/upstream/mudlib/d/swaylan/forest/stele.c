// /d/swaylan/forest/stele.c — 密林（AI23）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_push", "push");
}

/* 推石碑：地板一翻，跌進空地（空地 up 回來；使用者指示，2026-11-01）。 */
int do_push(string arg)
{
    object me = this_player();

    if( arg != "石碑" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N用力推了推石碑，只聽喀喇一聲，腳下的地板猛地一翻，$N整個人便跌進了地底。\n", me);
    if( !me->move("/d/swaylan/forest/clearing") ) return 0;
    message("vision", replace_string("頭頂的地板一翻，$N從上頭跌了下來。", "$N", me->name()) + "\n", environment(me), ({ me }));
    return 1;
}

void create()
{
    set("short", "密林");
    set("long", @LONG
密林深處空出一塊林地，正中立著一塊半人高的石碑，碑上的字跡早已模糊難辨，碑身卻被摸得光滑發亮，與四周長滿青苔的石頭大不相同。碑前的泥地上留著雜亂的腳印，到了碑前便突然斷了。
LONG
    );
    set("detail/石碑", "半人高的石碑，碑身被摸得光滑發亮，似乎常有人伸手去推 (push) 它。");
    set("exits", ([
        "west" : __DIR__"forest2",
    ]));
    set("objects", ([
        __DIR__"npc/bandit" : 2,
        __DIR__"npc/bandit_leader" : 1,
    ]));
    set("map/area", "密林");
    setup();
}
