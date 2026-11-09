// /d/taiyi/passage1.c — 通道（L8）。房間敘述依設計表需求撰寫（docs/AREA_TAIYI.md）。

#include <room.h>

inherit ROOM;

/* 往南走回井底（暗門）。 */
int valid_leave(object me, string dir)
{
    if( dir == "south" )
        tell_object(me, "你摸黑沿著通道往南走，從暗門鑽回了井底。\n");
    return ::valid_leave(me, dir);
}

void create()
{
    set("short", "通道");
    set("long", @LONG
暗門之後是一條狹窄的石砌通道，僅容一人側身而過，壁上插著半截殘燭，燭淚積了厚厚一層。空氣裏有股久未流通的潮味，腳步聲在石壁間來回碰撞，顯得格外清晰，不知已有多久沒人來過。
LONG
    );
    set("exits", ([
        "south" : __DIR__"well",
        "north" : __DIR__"passage2",
    ]));
    set("map/layer", "地下");
    set("map/area", "太乙觀");
    setup();
}
