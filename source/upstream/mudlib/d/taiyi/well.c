// /d/taiyi/well.c — 井底（L10）。房間敘述依設計表需求撰寫（docs/AREA_TAIYI.md）。

#include <room.h>

inherit ROOM;

/* 暗門找到之後，房間記住這件事，直到房間重置（使用者定案，2026-11-09）。 */
void reset()
{
    ::reset();
    delete_temp("door_found");
}

void init()
{
    ::init();
    add_action("do_climb", "climb");
    add_action("do_knock", "knock");
    add_action("do_push", "push");
}

/* climb 繩索 往上爬回後院。 */
int do_climb(string arg)
{
    object me = this_player();

    if( arg != "繩索" ) return notify_fail("這裡沒有什麼可以爬的。\n");
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    tell_object(me, "你抓緊粗麻繩，手腳並用地往上爬去。\n");
    message("vision", me->name() + "抓著繩索往上爬去。\n", this_object(), ({ me }));
    if( !me->move(__DIR__"yard") ) return 1;
    message("vision", me->name() + "從古井裏爬了上來。\n", environment(me), ({ me }));
    return 1;
}

/* knock 井壁：敲出暗門。 */
int do_knock(string arg)
{
    object me = this_player();

    if( arg != "井壁" ) return notify_fail("你要敲什麼？\n");
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    set_temp("door_found", 1);
    tell_object(me, "你屈指在井壁上一塊一塊地敲過去，敲到其中幾塊磚頭時聲音忽然空了，湊近細看，那幾塊磚頭與四周的並不相同，竟是一道暗門。\n");
    message("vision", me->name() + "屈指在井壁上一塊一塊地敲著。\n", this_object(), ({ me }));
    return 1;
}

/* push 暗門：進入通道（要先有人敲出暗門）。 */
int do_push(string arg)
{
    object me = this_player();

    if( arg != "暗門" && arg != "井壁" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    if( !query_temp("door_found") || arg != "暗門" )
        return notify_fail("你伸手推了推井壁，磚牆紋絲不動。\n");
    tell_object(me, "你伸手抵住那幾塊磚頭用力一推，暗門緩緩向內滑開，露出一條黑黝黝的通道，你側身鑽了進去。\n");
    message("vision", me->name() + "推開井壁上的一道暗門，側身鑽了進去。\n", this_object(), ({ me }));
    if( !me->move(__DIR__"passage1") ) return 1;
    message("vision", me->name() + "從暗門裏鑽了進來。\n", environment(me), ({ me }));
    return 1;
}

void create()
{
    set("short", "井底");
    set("long", @LONG
井底陰濕狹窄，抬頭只見一小圈天光，粗麻繩從上方垂落。井壁砌得整整齊齊，磚縫間滲著水氣，青苔滑膩膩地爬了滿牆，四下靜得只聽見水珠一滴一滴落下，回音在井中久久不散。
LONG
    );
    set("exits", ([
    ]));
    set("map/layer", "地下");
    set("map/area", "太乙觀");
    setup();
}
