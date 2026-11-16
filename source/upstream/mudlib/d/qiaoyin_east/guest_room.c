// /d/qiaoyin_east/guest_room.c — 白雲觀（B6）。依使用者設計表（2026-11-16）。

#include <room.h>

inherit ROOM;

// drink 茶杯：補水到上限的四分之一（使用者指示，2026-11-16）；茶壺裡的茶喝不完。
int do_drink(string arg)
{
    object me = this_player();
    int max, stuff;

    if( arg != "茶杯" && arg != "茶" && arg != "cup" && arg != "tea" ) return 0;
    max = me->query_stat_maximum("water");
    stuff = max / 4;
    if( stuff + me->query_stat("water") > max ) stuff = max - me->query_stat("water");
    if( stuff <= 0 ) return notify_fail("你已經漲得喝不下一滴水了。\n");
    me->supplement_stat("water", stuff);
    message_vision("$N端起茶杯，慢慢啜了幾口溫熱的茶水。\n", me);
    me->start_busy(1);
    return 1;
}

void create()
{
    set("short", "客房");
    set("long", @LONG
客房不大，一張木榻、一張方桌，被褥疊得整整齊齊，帶著曬過的乾草味。桌上的粗瓷茶壺與兩個茶杯擦得乾淨，想是為借宿的香客與過路人預備的。窗外有一枝松枝探進來，在窗欞上擦出細細的聲響。
LONG
    );
    set("exits", ([
        "east" : __DIR__"sutra_hall",
    ]));
    create_door("east", "房門", "west", DOOR_CLOSED);
    set("detail/茶杯", "兩個粗瓷茶杯擦得乾乾淨淨，杯裏盛著溫熱的茶水，茶香淡淡的，看來可以倒來喝 (drink) 上一口解解渴。\n");
    set("map/area", "白雲觀");
    setup();
}
