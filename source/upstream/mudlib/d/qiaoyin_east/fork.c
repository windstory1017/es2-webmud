// /d/qiaoyin_east/fork.c — 喬陰東郊（J12）。依使用者設計表（2026-11-16）。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_pass", "pass");
}

int do_pass(string arg)
{
    if( arg != "草叢" && arg != "grass" ) return 0;
    message_vision("$N彎下身撥開草叢，一低頭鑽了進去。\n", this_player());
    this_player()->move(__DIR__"thicket");
    message("vision", this_player()->name() + "從草叢外鑽了進來。\n",
        environment(this_player()), this_player());
    return 1;
}

void create()
{
    set("short", "岔路口");
    set("long", @LONG
路口立著一塊被磨得發白的界石，上頭的字跡早已難辨。路邊長著一大片半人高的草叢，草葉糾結成團，看不清底下藏著什麼；另一頭的小徑被踩得平整，隱約有藥草的苦香飄來，與荒草的氣味混在一處。
LONG
    );
    set("exits", ([
        "westup" : __DIR__"mountain_road",
        "south" : __DIR__"hut",
    ]));
    set("detail/草叢", "草叢又高又密，底下卻有被人踩出來的痕跡，應該可以穿 (pass) 過去。\n");
    set("map/area", "喬陰東郊");
    setup();
}
