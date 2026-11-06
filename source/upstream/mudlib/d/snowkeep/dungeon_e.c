// /d/snowkeep/dungeon_e.c — 雪吟莊（J2）。依使用者提供的雪吟莊設計表（2026-11-06）；敘述：新寫。
// 牢門從裡面不用鑰匙也打得開（open door），免得有人趁門開著進來、房間重置後被鎖在裡面出不去。
// 區域文件見 docs/AREA_SNOWKEEP.md。

#include <room.h>

inherit ROOM;

#define LOCK_ID "snowkeep_dungeon"

varargs int open_door(string dir, int from_other_side)
{
    mapping doors = query_doors();

    if( !from_other_side && dir == "west" && mapp(doors) && mapp(doors["west"])
    &&  (doors["west"]["status"] & DOOR_LOCKED) ) {
        unlock_door("west", LOCK_ID);
        if( this_player() ) tell_object(this_player(), "你摸到牢門內側的門閂﹐把它拔了開來。\n");
    }
    return ::open_door(dir, from_other_side);
}

void create()
{
    set("short", "地牢深處");
    set("long", @LONG
這是地牢最深處的一間石牢，三面都是堅硬的山岩，一面是手臂粗的鐵條。石牢裏收拾得意外乾淨，角落擺著一張石床，牆上卻刻滿了密密麻麻的劃痕，看來被關在這裡的人，已在暗無天日之中熬過了不少日子。
LONG
    );
    set("exits", ([
        "west" : __DIR__"dungeon",
    ]));
    set("objects", ([
        "/custom/legacy_npc/yu_lan_jian_yi" : 1,
    ]));
    set("map/area", "雪吟莊");
    set("map/layer", "地下");
    // 與地牢（H2）之間的牢門，兩邊狀態一致（見 dungeon.c）。
    create_door("west", ([ "name" : "牢門", "id" : ({ "west", "牢門", "door" }), "other_side_dir" : "east",
        "status" : DOOR_CLOSED | DOOR_LOCKED, "lock" : "snowkeep_dungeon" ]));
    setup();
}
