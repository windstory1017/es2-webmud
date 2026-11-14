// /d/shuiyue/sky_tree.c — 水月村（F24）。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_enter", "climb");
}

int do_enter(string arg)
{
    object me = this_player();

    if( arg != "丱天樹" && arg != "卯天樹" && arg != "樹" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N手腳並用，順著樹幹攀了上去。\n", me);
    if( !me->move("/d/shuiyue/tree_top") ) return 0;
    message("vision", replace_string("$N從樹下爬了上來，喘了一口大氣。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void create()
{
    set("short", "『丱天樹』");
    set("long", @LONG
丱天樹, 相傳是在泰武定國, 與香雪國, 兩國戰爭時, 天變異象, 巨雷轟至土地上, 方圓百里之內無一物倖存, 唯獨這棵當時還是棵小樹的丱天樹仍然屹立不搖不受影響, 後來陸陸續續有戰後人民遷移至此處, 傳聞才慢慢傳開, 而這棵樹木形狀像人的一雙手向上伸展的樹木, 居民於是取名為丱天樹。
LONG
    );
    set("detail/丱天樹", "樹幹粗壯，盤根錯節，藤蔓與樹瘤形成一處處天然的落腳點，看起來可以爬 (climb) 上去。");
    set("exits", ([
        "west" : __DIR__"mud_road",
        "north" : __DIR__"woods",
    ]));
    set("map/area", "水月村");
    setup();
}
