// /d/snowkeep/dungeon.c — 雪吟莊（H2）地牢。依使用者提供的雪吟莊設計表（2026-11-06）；敘述：新寫。
// 往東（地牢深處 J2）是上鎖的牢門（標準門，關著時出口不顯示）：用黃銅鑰匙（魚鐵山身上）
// unlock door with key、open door 打開；獄卒在場時仍會攔人。房間重置時牢門重新關上鎖好（使用者指示，2026-11-06）。
// 區域文件見 docs/AREA_SNOWKEEP.md。

#include <room.h>

inherit ROOM;

#define LOCK_ID "snowkeep_dungeon"

int valid_leave(object me, string dir)
{
    object jailer;

    if( dir == "east" ) {
        jailer = present("jailer", this_object());
        if( objectp(jailer) && living(jailer) && jailer->query("snowkeep_jailer") && jailer != me )
            return notify_fail(jailer->name() + "伸手一攔﹐冷冷地說道：「沒有莊主的吩咐﹐誰也不准過去。」\n");
    }
    return ::valid_leave(me, dir);
}

/* 房間重置：牢門重新關上、鎖好（兩邊一起）。 */
void reset()
{
    mapping doors;

    ::reset();
    doors = this_object()->query_doors();
    if( !mapp(doors) || !mapp(doors["east"]) ) return;
    if( !(doors["east"]["status"] & DOOR_CLOSED) ) catch(close_door("east"));
    if( !(doors["east"]["status"] & DOOR_LOCKED) ) catch(lock_door("east", LOCK_ID));
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
    create_door("east", ([ "name" : "牢門", "id" : ({ "east", "牢門", "door" }), "other_side_dir" : "west",
        "status" : DOOR_CLOSED | DOOR_LOCKED, "lock" : LOCK_ID ]));
    setup();
}
