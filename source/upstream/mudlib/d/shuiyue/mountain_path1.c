// /d/shuiyue/mountain_path1.c — 水月村（D32）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "上山小路");
    set("long", @LONG
小路沿著山坡蜿蜒而上，鋪路的石塊被磨得圓潤，縫隙間長滿了青苔。兩旁樹木漸漸濃密，林濤如潮，遠遠夾著溪水奔流的聲音，走一步便覺得空氣又涼了一分，像是正要走進雲霧裡。
LONG
    );
    set("exits", ([
        "northup" : __DIR__"village2",
        "southdown" : __DIR__"mountain_path2",
    ]));
    set("map/area", "水月村");
    setup();
    replace_program(ROOM);
}
