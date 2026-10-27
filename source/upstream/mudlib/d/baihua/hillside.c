// /d/baihua/hillside.c — 百花村（F10）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void init()
{
    add_action("do_search", "search");
}

int do_search(string arg)
{
    object me = this_player();

    if( !arg || (arg != "草叢" && arg != "grass") ) {
        write("到處都是比人還高的草叢，仔細找找或許可以找到出路。\n");
        return 1;
    }
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法分心找路。\n");
    message_vision("$N撥開比人還高的雜草，仔細地找了一陣，終於發現一條被草掩住的小徑。\n", me);
    if( !me->move(__DIR__"trail") ) return 0;
    message("vision", me->name() + "撥開草叢鑽了出來。\n", environment(me), me);
    return 1;
}

void create()
{
    set("short", "山腰上");
    set("long", @LONG
這裡的坡度漸漸變得平緩，可是兩旁的雜草生得極其茂密，高度比人還高，不知不覺你發現自己已經找不到路徑了！遠處傳來陣陣的狼嚎聲，你不禁開始緊張了起來。在這種地方迷了路，那可不是件好玩的事，趕緊找出路吧。
LONG
    );
    set("exits", ([
        "northwest" : "/d/oldpine/grass2",
    ]));
    set("objects", ([
        "/d/oldpine/npc/goat" : 3,
    ]));
    set("outdoors", "baihua");
    set("map/area", "百花村");
    setup();
}
