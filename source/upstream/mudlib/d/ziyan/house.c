// /d/ziyan/house.c — 紫煙鎮（M10）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_search", "search");
}

/* 搜屋：先看到書櫃旁的血手印，搜書櫃找到血書（每次房間重置只有一封）。 */
int do_search(string arg)
{
    object me = this_player(), letter;

    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    if( !arg ) {
        write("你仔細觀察屋內，發現書櫃側邊有個不起眼的血手印，看起來似乎不太一般。\n");
        return 1;
    }
    if( arg != "書櫃" ) return 0;
    if( query_temp("letter_taken") ) {
        write("你把書櫃翻了個遍，再也找不到什麼。\n");
        return 1;
    }
    set_temp("letter_taken", 1);
    message_vision("$N在書櫃中發現一封血書。\n", me);
    letter = new("/d/ziyan/obj/bloody_letter");
    if( !letter->move(me) ) letter->move(this_object());
    return 1;
}

void reset()
{
    ::reset();
    delete_temp("letter_taken");
}

void create()
{
    set("short", "民宅");
    set("long", @LONG
屋內一片狼藉，桌椅東倒西歪，地上的血跡雖已被清洗過，石板縫裡仍留著洗不淨的暗紅。這裡是近日發生兇案的現場，官府草草收拾後便貼上封條了事，倒是靠牆的書櫃還立得端端正正。
LONG
    );
    set("detail", ([
        "血手印" : "書櫃旁這道倉促留下的手印，明顯有古怪。",
    ]));
    set("exits", ([
        "east" : __DIR__"lane",
    ]));
    set("map/area", "水嵐縣");
    setup();
}
