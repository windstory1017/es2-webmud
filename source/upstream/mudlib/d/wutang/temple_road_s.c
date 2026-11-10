// /d/wutang/temple_road_s.c — 五堂鎮（X8）。房間敘述依設計表需求擴寫。

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

    if( arg != "小巷子" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N側著身子擠進了小巷子。\n", me);
    if( !me->move("/d/wutang/dark_alley_e") ) return 0;
    message("vision", replace_string("$N從小巷子裡擠了出來。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void create()
{
    set("short", "廟口小路");
    set("long", @LONG
往北通往鎮天神廟的石板路，路旁擺著幾個賣香燭紙錢的小攤，空氣裡瀰漫著檀香的氣味。路邊兩棟房子中間夾著一條狹窄的小巷子，看不清裡頭有些什麼。往南走就是熱鬧的雲棧市集廣場。
LONG
    );
    set("detail/小巷子", "兩棟房子中間夾著一條狹窄的小巷子，看不清裡頭有些什麼，側著身子可以擠 (enter) 進去。");
    set("exits", ([
        "north" : __DIR__"temple_road_n",
        "south" : __DIR__"market_square",
    ]));
    set("map/area", "五堂鎮");
    setup();
}
