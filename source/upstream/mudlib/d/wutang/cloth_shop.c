// /d/wutang/cloth_shop.c — 五堂鎮（T10）。房間敘述依設計表需求擴寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "布莊");
    set("long", @LONG
一家不大的布莊，架子上堆滿了一疋疋的布料，從粗棉布到上好的綢緞應有盡有。櫃臺上攤開著幾疋顏色鮮豔的花布，是老闆娘李美麗最得意的貨色，常有姑娘家在這裡流連忘返。往南可以回到街道。
LONG
    );
    set("exits", ([
        "south" : __DIR__"east_street3",
    ]));
    set("objects", ([
        __DIR__"npc/tao" : 1,
        __DIR__"npc/young_gentleman" : 1,
        __DIR__"npc/bai_ou" : 1,
    ]));
    set("map/area", "五堂鎮");
    setup();
    replace_program(ROOM);
}
