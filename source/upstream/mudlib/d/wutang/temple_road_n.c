// /d/wutang/temple_road_n.c — 五堂鎮（X6）。房間敘述依設計表需求擴寫。

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

    if( arg != "暗巷" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N左右張望了一下，閃身鑽進了暗巷。\n", me);
    if( !me->move("/d/wutang/yan_gate") ) return 0;
    message("vision", replace_string("$N從暗巷裡走了出來。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void create()
{
    set("short", "廟口小路");
    set("long", @LONG
通往鎮天神廟的石板小路，往來的香客絡繹不絕，人聲雖然鼎沸，越往北走卻越顯得莊嚴肅穆。路的西邊是最近出了事的顏家大宅，高牆下有一條陰暗的巷子，鎮民經過時都刻意繞開。東邊有條小路通往民房，往南則回到廟口。
LONG
    );
    set("detail/暗巷", "顏家大宅高牆下的一條陰暗巷子，鎮民經過時都刻意繞開，膽子夠大的話，可以鑽 (enter) 進去看看。");
    set("exits", ([
        "north" : __DIR__"temple",
        "south" : __DIR__"temple_road_s",
        "east" : __DIR__"path_e",
    ]));
    set("map/area", "五堂鎮");
    setup();
}
