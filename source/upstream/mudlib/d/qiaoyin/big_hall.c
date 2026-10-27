// /d/qiaoyin/big_hall.c — 喬陰縣（P8）。依使用者設計表（2026-10-16）。

#include <room.h>

// 改為龍圖藥鋪櫃台：可查丹藥、買丹藥與配方、賣丹藥（使用者指示 #3，2026-10-23）。
inherit HERB;

// 煉丹房（往北）只讓方士進入，並要先向江述仁繳交柴火錢（使用者設計，2026-10-16）。
// 繳費記在玩家的暫存資料，登出後失效；不登出就能反覆進出。
int valid_leave(object me, string dir)
{
    if( dir == "north" && userp(me) ) {
        if( me->query_class() != "alchemist" )
            return notify_fail("一名童子伸手攔住你：「丹房重地，只有方士才能進去。」\n");
        if( !me->query_temp("qiaoyin/alchemy_paid") )
            return notify_fail("一名童子伸手攔住你：「要進煉丹房，得先向江大師兄繳交柴火錢。」\n");
    }
    return ::valid_leave(me, dir);
}

void create()
{
    set("short", "大屋正廳");
    set("long", @LONG
進了這間大屋才發現，原來這裡是間方士們煉丹燒爐的丹房，大廳中擺設很簡單，只有一些木櫃跟香煙裊繞的青銅丹爐，幾名童子正用蒲扇在丹爐旁搧火，另外一些童子則蹲在一旁用瓷缽研磨不知名的藥物。
LONG
    );
    set("exits", ([
        "north" : __DIR__"alchemy_room",
        "east" : __DIR__"alley",
    ]));
    set("objects", ([
        __DIR__"npc/jiang_hsuren" : 1,
    ]));
    set("map/area", "喬陰縣");
    setup();
}
