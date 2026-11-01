// /d/ziyan/yamen_hall.c — 紫煙鎮（E10）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

#define KEY_LOCK "ziyan_inner_hall"   // 石鑰匙的鎖名，與內廳木門相同

void init()
{
    ::init();
    add_action("do_push", "push");
}

/* 按金蟾的眼睛：掉出一把石鑰匙，每次房間重置只掉一把。 */
int do_push(string arg)
{
    object me = this_player(), key;

    if( arg != "眼睛" && arg != "金蟾眼睛" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    if( query_temp("key_dropped") ) {
        message_vision("$N伸手按了按金蟾的眼睛，卻再也沒有動靜。\n", me);
        return 1;
    }
    set_temp("key_dropped", 1);
    message_vision("$N伸手按了按金蟾的眼睛，只聽喀的一聲，金蟾嘴裡掉出一把石鑰匙。\n", me);
    key = new("/d/ziyan/obj/stone_key");
    if( !key->move(me) ) key->move(this_object());
    return 1;
}

void reset()
{
    ::reset();
    delete_temp("key_dropped");
    // 往內廳的木門重新關上、上鎖。
    set_door("south", "status", DOOR_CLOSED | DOOR_LOCKED);
    if( objectp(find_object("/d/ziyan/inner_hall")) )
        "/d/ziyan/inner_hall"->set_door("north", "status", DOOR_CLOSED | DOOR_LOCKED);
}

void create()
{
    set("short", "大堂");
    set("long", @LONG
這裡名為大堂，看起來卻更像是縣令的書房，案上堆著幾本帳冊，公文倒沒見幾件。案頭擺著一尊三足金蟾的木像，雕工精細，一雙眼睛尤其靈動，彷彿正盯著來人。堂後的木門上掛著一把銅鎖。
LONG
    );
    set("detail", ([
        "木像" : "一只象徵財富與繁榮的招財吉祥物，顯見趙弘春相當注重身外之物。木像的做工相當不錯，尤其眼睛特別靈動。",
        "三足金蟾" : "一只象徵財富與繁榮的招財吉祥物，顯見趙弘春相當注重身外之物。木像的做工相當不錯，尤其眼睛特別靈動。",
    ]));
    set("exits", ([
        "east" : __DIR__"yamen_gate",
        "north" : __DIR__"jail",
        "south" : __DIR__"inner_hall",
    ]));
    set("map/area", "水嵐縣");
    // 大堂與內廳之間的木門，平時上鎖，用大堂金蟾掉出的石鑰匙打開。
    create_door("south", ([ "name" : "木門", "id" : ({ "south", "木門", "door" }), "other_side_dir" : "north",
        "status" : DOOR_CLOSED | DOOR_LOCKED, "lock" : "ziyan_inner_hall" ]));
    setup();
}
