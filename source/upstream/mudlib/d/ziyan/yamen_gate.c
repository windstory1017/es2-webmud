// /d/ziyan/yamen_gate.c — 紫煙鎮（G10）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_knock", "knock");
}

/* 擊鼓：衙役衝出來把人架進大堂（大門不能直接走進去）。 */
int do_knock(string arg)
{
    object me = this_player();

    if( arg != "牛皮大鼓" && arg != "大鼓" && arg != "鼓" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N拿起鼓槌，咚咚咚地擂起牛皮大鼓。\n", me);
    message_vision("縣衙大門猛地打開，幾名衙役衝了出來，不由分說地架起$N往裡頭拖去。\n", me);
    if( !me->move("/d/ziyan/yamen_hall") ) return 0;
    message("vision", replace_string("幾名衙役架著$N走了進來，往堂上一推，便又退了出去。", "$N", me->name()) + "\n", environment(me), ({ me }));
    return 1;
}

void create()
{
    set("short", "縣衙大門");
    set("long", @LONG
紫煙鎮縣衙大門，兩扇朱漆大門緊緊閉著，門前蹲著一對石獅，神情倨傲。門邊架著一面牛皮大鼓，鼓面上落了一層薄灰，鼓槌斜斜地掛在架上，看來已經許久沒有人敢來擊鼓鳴冤了。
LONG
    );
    set("exits", ([
        "north" : __DIR__"yamen_street",
    ]));
    set("objects", ([
        __DIR__"npc/yamen_guard" : 2,
    ]));
    set("map/area", "紫煙鎮");
    setup();
}
