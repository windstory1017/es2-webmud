// /d/oldpine/cave_mouth.c — 迷霧森林（Z12）。房間敘述依設計表提示撰寫。

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

    // 只有 enter 洞口 才進洞；其他寫法交還給系統的 enter 指令。
    if( arg != "洞口" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    message_vision("$N彎腰進入了眼前森然的洞口。\n", me);
    if( !me->move(__DIR__"cave1") ) return 0;
    message("vision", replace_string("$N從透露著光亮的洞口走了進來。", "$N", me->name()) + "\n", environment(me), me);
    return 1;
}

void create()
{
    set("short", "山洞口");
    set("long", @LONG
獸徑的盡頭是一面陡峭的山壁，山壁下有一個黑漆漆的洞口，洞口的岩石被磨得光滑，顯然常有東西進出。洞口前散落著吃剩的蜂巢和魚骨，還有一些被扒開的泥土。洞裡隱約傳出低沉的鼾聲和咕嚕聲，讓人遲疑該不該進去....
LONG
    );
    set("detail/洞口", "這個岩石洞口看起來深邃，並且在旁邊散落著一些殘餘的動物殘骸，似乎裡面有某些傷害性很強的動物存在。");
    set("exits", ([
        "west" : __DIR__"beast_trail",
    ]));
    set("map/area", "迷霧森林");
    setup();
}
