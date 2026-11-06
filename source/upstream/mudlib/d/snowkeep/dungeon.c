// /d/snowkeep/dungeon.c — 雪吟莊（H2）地牢。依使用者提供的雪吟莊設計表（2026-11-06）；敘述：新寫。
// 往東（地牢深處 J2）：獄卒在場時攔人；獄卒不在時，身上要有黃銅鑰匙（魚鐵山身上）才打得開牢門，鑰匙不會用掉。
// 區域文件見 docs/AREA_SNOWKEEP.md。

#include <room.h>

inherit ROOM;

int valid_leave(object me, string dir)
{
    object jailer;

    if( dir == "east" ) {
        jailer = present("jailer", this_object());
        if( objectp(jailer) && living(jailer) && jailer->query("snowkeep_jailer") && jailer != me )
            return notify_fail(jailer->name() + "伸手一攔﹐冷冷地說道：「沒有莊主的吩咐﹐誰也不准過去。」\n");
        if( !present("brass key", me) )
            return notify_fail("牢門上掛著一把沉甸甸的大鎖﹐鎖得緊緊的﹐怎麼推也推不開。\n");
        tell_object(me, "你掏出黃銅鑰匙打開大鎖﹐推開了沉重的牢門。\n");
    }
    return ::valid_leave(me, dir);
}

void create()
{
    set("short", "地牢");
    set("long", @LONG
甬道兩旁是一間間鐵柵隔成的牢房，大多空著，地上鋪著發黴的稻草，散發出一股刺鼻的霉味。甬道中央擺著一張木桌和一把木椅，桌上一盞油燈、半碗冷飯，東邊一道包著鐵皮的厚重牢門上，掛著一把沉甸甸的大鎖。
LONG
    );
    set("exits", ([
        "west" : __DIR__"dungeon_w",
        "east" : __DIR__"dungeon_e",
    ]));
    set("objects", ([
        __DIR__"npc/jailer" : 1,
    ]));
    set("map/area", "雪吟莊");
    set("map/layer", "地下");
    setup();
}
