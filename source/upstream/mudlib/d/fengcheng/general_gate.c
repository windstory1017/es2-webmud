// /d/fengcheng/general_gate.c — 風城（H14）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "將軍府");
    set("long", @LONG
將軍府大門高大威嚴，兩尊石獅蹲踞階前，門上銅釘排得整整齊齊，朱漆在歲月裡褪去了幾分光澤。府內庭院深深，隱約有練兵的呼喝與兵器相擊聲傳出，門前守衛目光銳利，不容閒人亂闖。
LONG
    );
    set("exits", ([
        "south" : __DIR__"street2",
        "north" : __DIR__"inner_yard",
    ]));
    set("objects", ([
        __DIR__"npc/gate_guard" : 2,
    ]));
    set("no_fight", 1);   // 非戰區
    set("map/area", "風城");
    setup();
}

// 沒猜對小孩的字謎，門衛不讓人往北進內院廣場。
int valid_leave(object me, string dir)
{
    object guard;

    if( dir == "north" && !me->query("quest/fengcheng_riddle") ) {
        foreach(guard in all_inventory(this_object()))
            if( guard->query("gate_guard") && living(guard) && me->visible(guard) ) {
                guard->warn(me);
                return notify_fail(guard->name() + "把你攔了下來。\n");
            }
    }
    return ::valid_leave(me, dir);
}
