// /d/shuiyue/waterfall.c — 水月村（F20）。

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

    if( arg != "水月瀑布" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N側身穿過水簾，鑽進了瀑布的後方。\n", me);
    if( !me->move("/d/shuiyue/cave") ) return 0;
    message("vision", replace_string("$N從瀑布的水簾外鑽了進來，身上濺了不少水珠。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void create()
{
    set("short", "水月瀑布");
    set("long", @LONG
展現在你眼前的就是著名的水月瀑布了, 澄澈透明的泉水, 從五六十尺高的山壁奔流而下, 墜入沁綠的深潭中, 氣勢浩蕩, 吸引了不少遊客在此佇足觀賞, 有時午後下過雨後會出現七色彩虹, 更是一大美景。瀑布四面皆有高聳的山壁圍繞, 蔓綠的藤草, 任意攀附生長, 到處都是水霧迷漫, 氣嵐煙騰。
LONG
    );
    set("detail/水月瀑布", "水簾後方似乎藏著一處空間，可以進入 (enter) 一探究竟。");
    set("exits", ([
        "south" : __DIR__"woods",
    ]));
    set("map/area", "水月村");
    setup();
}
