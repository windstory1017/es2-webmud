// /d/swaylan/forest/hideout.c — 密林（AG29）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_kick", "kick");
}

/* 踢角落的石頭：石牆移開，進入密室（密室 out 回來）。 */
int do_kick(string arg)
{
    object me = this_player();

    if( arg != "石頭" && arg != "石子" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N一腳踢在角落的石頭上，只聽轟隆一聲，石牆緩緩移開，露出一道暗門。\n", me);
    message_vision("$N閃身鑽進暗門，石牆又在身後轟隆隆地合上了。\n", me);
    if( !me->move("/d/swaylan/forest/chamber") ) return 0;
    message("vision", replace_string("石牆轟隆一聲移開，$N從暗門外走了進來。", "$N", me->name()) + "\n", environment(me), ({ me }));
    return 1;
}

void create()
{
    set("short", "地下密道");
    set("long", @LONG
密道到這裡變得寬敞，像是一間地下大廳，幾張長桌上散落著酒罈和骰子，牆上掛著幾面破舊的官兵旗幟，被人用刀劃得稀爛。角落裡有一塊石頭，形狀方正得出奇，看起來不太像是天然的。
LONG
    );
    set("exits", ([
        "northeast" : __DIR__"tunnel",
    ]));
    set("objects", ([
        __DIR__"npc/bandit_chief" : 1,
        __DIR__"npc/bandit_captain" : 1,
    ]));
    set("map/area", "密林");
    set("map/layer", "地下");
    setup();
}
