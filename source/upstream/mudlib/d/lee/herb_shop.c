// /d/lee/herb_shop.c
// 李家村復原版。CONFIRMED_MIXED
// Topology follows the user-provided Li Village map; generated prose is intentionally replaceable.

#include <room.h>

inherit HERB;

void create()
{
    set("short", "藥鋪");
    set("long", @LONG
這是一間不大的藥鋪，木櫃裡分門別類收著藥材與丹藥，空氣中混著草根與藥香。掌櫃李岩守著櫃臺，店門朝東開向石子路。牆上掛著幾串曬乾的藥草，屋樑上懸著一副舊招牌，字跡已經有些斑駁老舊。鋪子裡賣的丹藥多是聶晟閒暇時煉的，價格公道，村民生病時多半先來這裡瞧瞧。
LONG
    );
    set("objects", ([
        "/d/lee/npc/lee_yen" : 1
    ]));
    set("exits", ([
        "east" : __DIR__"nroad1"
    ]));
    setup();
    set("map/area", "李家村");
}
