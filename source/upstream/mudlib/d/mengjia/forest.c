// /d/mengjia/forest.c — 孟家莊（F10）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "密林");
    set("long", @LONG
沙土地上滿是未乾的血跡，四周的古松上也被濺上了鮮血。在往北走，就是孟家的祖墳了，據說裏面葬的是天朝的一位開國將軍，五虎斷門刀的創使人，孟翔溪。
LONG
    );
    set("exits", ([
        "south" : __DIR__"street",
        "north" : __DIR__"graveyard",
    ]));
    set("objects", ([
        __DIR__"npc/white_tiger" : 1,
    ]));
    set("map/area", "孟家莊");
    setup();
}

// 白額虎還活著，往北進孟家祖墳的路就被擋住；打死之後才過得去（重生後又會擋路）。
int valid_leave(object me, string dir)
{
    object ob;

    if( dir == "north" ) {
        foreach(ob in all_inventory(this_object()))
            if( ob->query("white_tiger") && living(ob) && me->visible(ob) ) {
                message_vision("$N低吼著擋在$n面前﹐不讓$n往北去。\n", ob, me);
                return 0;
            }
    }
    return ::valid_leave(me, dir);
}
