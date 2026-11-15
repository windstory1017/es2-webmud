// /d/tianshan/secret_room.c — 天山（T2 密室）。
// 流程：look 字跡 → wipe 灰塵 → （學內八套；柳家拳內八套還沒有設計，目前 learn 只是提示，之後再開）。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_wipe", "wipe");
    add_action("do_learn", "learn");
}

int do_wipe(string arg)
{
    object me = this_player();

    if( arg != "灰塵" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N伸手抹去了牆上的灰塵。\n", me);
    me->set_temp("tianshan/wiped", 1);
    tell_object(me, "牆上有一些內八套的武功招式，你可以學學看 (learn)。\n");
    return 1;
}

// 柳家拳內八套的技能還沒有設計（使用者決定先做房間，2026-11-14），這裡先不傳授。
int do_learn(string arg)
{
    object me = this_player();

    if( arg != "內八套" ) return 0;
    if( !me->query_temp("tianshan/wiped") )
        return notify_fail("牆上只有一片模糊的字跡，你什麼也看不出來。\n");
    return notify_fail("你對著牆上的招式琢磨了半天，卻怎麼也看不出其中的門道。\n");
}

void create()
{
    set("short", "密室");
    set("long", @LONG
陰沉沉的屋子，光線從上面照下來，照在一面牆上。牆上似乎有些模糊的字跡。
LONG
    );
    set("detail/字跡", "字跡上灰塵太多, 你可以試著抹去 (wipe) 灰塵");
    set("exits", ([
        "up" : __DIR__"hall",
    ]));
    set("map/area", "天山");
    set("map/layer", "密室");
    setup();
}
