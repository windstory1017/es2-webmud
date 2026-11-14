// /d/shuiyue/tree_top.c — 水月村（H24）。

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

    if( arg != "樹洞" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N彎腰鑽進了樹洞。\n", me);
    if( !me->move("/d/shuiyue/tree_hollow") ) return 0;
    message("vision", replace_string("$N從樹洞外鑽了進來。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void create()
{
    set("short", "樹上");
    set("long", @LONG
這裡是樹上, 你隱隱約約有聽到有人在喊叫, 附近亦無人在場, 令你不禁有點發寒, 四周圍陰冷潮溼, 沒有什麼特別的, 只有你眼前的樹洞, 和一條條枯藤, 以及一些青苔。
LONG
    );
    set("detail/樹洞", "樹幹上裂開一個黑漆漆的樹洞，大小足夠一個人鑽 (enter) 進去。");
    set("exits", ([
        "down" : __DIR__"sky_tree",
    ]));
    set("objects", ([
        "/d/oldpine/npc/squirrel" : 4,
    ]));
    set("map/area", "水月村");
    set("map/layer", "樹上");
    setup();
}
