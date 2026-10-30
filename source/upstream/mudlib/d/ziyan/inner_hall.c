// /d/ziyan/inner_hall.c — 紫煙鎮（E12）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    if( userp(this_player()) ) call_out("call_for_help", 0, this_player());
}

/* 有人闖進內廳：趙弘春大喊，叫出 2 名衙役、2 名捕快出手。
 * 趙弘春每活一次只喊一次（記在他身上），多人進來也只叫一組。 */
void call_for_help(object me)
{
    object zhao, ob;
    int i;

    if( !objectp(me) || environment(me) != this_object() ) return;
    if( !objectp(zhao = present("zhao hong chun", this_object())) || !living(zhao) ) return;
    if( zhao->query_temp("called_help") ) return;
    zhao->set_temp("called_help", 1);
    message_vision("$N大喊：「誰！來人啊～！」\n", zhao);
    for( i = 0; i < 4; i++ ) {
        ob = new(i < 2 ? "/d/ziyan/npc/yamen_guard" : "/d/ziyan/npc/constable");
        ob->move(this_object());
        message_vision("$N聞聲衝了進來。\n", ob);
        ob->kill_ob(me);
    }
}

void create()
{
    set("short", "內廳");
    set("long", @LONG
一間雅致的內廳，四壁的多寶格上擺滿了古玩珍寶，青銅鼎、白玉璧、鑲金的如意，件件價值不菲，與紫煙鎮街上的破落簡直是兩個天地。屋裡燃著上好的沉香，香氣濃得有些膩人。
LONG
    );
    set("exits", ([
        "north" : __DIR__"yamen_hall",
    ]));
    set("objects", ([
        "/custom/legacy_npc/zhao_hong_chun" : 1,
    ]));
    set("map/area", "紫煙鎮");
    // 大堂與內廳之間的木門，平時上鎖，用大堂金蟾掉出的石鑰匙打開。
    create_door("north", ([ "name" : "木門", "id" : ({ "north", "木門", "door" }), "other_side_dir" : "south",
        "status" : DOOR_CLOSED | DOOR_LOCKED, "lock" : "ziyan_inner_hall" ]));
    setup();
}
