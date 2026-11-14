// /d/mengjia/graveyard.c — 孟家莊（F8）。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_roll", "roll");
}

int do_roll(string arg)
{
    object me = this_player();

    if( arg != "石像" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N雙手抵住石像用力一轉﹐只聽轟的一聲悶響﹐腳下的石板翻了開來﹐$N跌了下去。\n", me);
    if( !me->move("/d/mengjia/tomb") ) return 0;
    message("vision", replace_string("$N從上面跌了下來。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void create()
{
    set("short", "孟家祖墳");
    set("long", @LONG
這裡就是孟家的墳地，一塊塊墓碑上記載著孟家先人的功績。在墳地的最北面，有一個比較大的墳墓，由於幾百年的風吹雨打，已經看不清墓碑上所刻的文字了。墳墓的前面立著一個半人半鬼的石像，令人有種不祥的預感。
LONG
    );
    set("detail/石像", "上身如虎下身似人，猙獰的面孔讓人不敢多看，這個石像好像可以轉動 (roll)。");
    set("exits", ([
        "south" : __DIR__"forest",
    ]));
    set("map/area", "孟家莊");
    setup();
}
