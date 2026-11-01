// /d/swaylan/smithy.c — 水嵐縣（W14）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "鐵匠鋪");
    set("long", @LONG
一進門便是一股熱浪撲面而來，爐火燒得通紅，鐵砧上叮叮噹噹的敲打聲震得人耳朵發麻。牆上掛滿了打好的短劍、匕首和鋤頭，角落的木桶裡浸著剛淬過火的鐵器，嘶嘶地冒著白煙。
LONG
    );
    set("exits", ([
        "west" : __DIR__"north_street2",
    ]));
    set("objects", ([
        __DIR__"npc/smith" : 1,
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
