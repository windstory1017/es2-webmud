// /d/swaylan/crossroad.c — 水嵐縣（S12）。房間敘述依設計表需求撰寫。往北是冷梅莊大門。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "路口");
    set("long", @LONG
城北的一處路口，一條碎石小路從這裡岔出，兩旁種滿了梅樹，枝幹虯曲，一路蜿蜒而上，盡頭想必便是水嵐縣第一大戶冷梅莊。路旁的雜木林裡不時傳來砍柴的聲響，篤篤地傳得很遠。
LONG
    );
    set("exits", ([
        "west" : __DIR__"woods1",
        "southeast" : __DIR__"north_street2",
        "north" : __DIR__"lunmay/gate",
    ]));
    set("objects", ([
        "/d/wutang/npc/apprentice" : 2,
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
