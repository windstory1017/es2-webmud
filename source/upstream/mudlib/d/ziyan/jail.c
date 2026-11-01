// /d/ziyan/jail.c — 紫煙鎮（E8）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

/* 林育崙還活著的時候，不讓人離開牢房。 */
int valid_leave(object me, string dir)
{
    object lin;

    if( dir == "south" && userp(me)
    &&  objectp(lin = present("lin yu lun", this_object())) && living(lin) )
        return notify_fail("林育崙橫刀攔住你的去路：「進了這裡，還想出去？」\n");
    return ::valid_leave(me, dir);
}

void create()
{
    set("short", "牢房");
    set("long", @LONG
陰暗潮濕的牢房，石牆上滲著水珠，地上鋪著發霉的稻草，空氣中瀰漫著腐臭與血腥的氣味。牢門用碗口粗的木柵圍成，守衛森嚴，被關進這裡的人，多半再也沒有走出去過。
LONG
    );
    set("exits", ([
        "south" : __DIR__"yamen_hall",
    ]));
    set("objects", ([
        "/custom/legacy_npc/lin_yu_lun" : 1,
        __DIR__"npc/constable" : 2,
    ]));
    set("map/area", "水嵐縣");
    setup();
}
