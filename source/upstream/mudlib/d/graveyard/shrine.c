// /d/graveyard/shrine.c — 亂葬崗（F2）。房間敘述舊資料全文引用。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "祠堂");
    set("long", @LONG
這裡是一間破舊的小祠堂﹐屋頂已經被風掀走了一大塊﹐牆壁上的
灰泥也剝落了許多﹐不過安放牌位的桌子跟祭祀用的物品倒是不缺﹐你
可以看到前面供的牌位上寫的是「乙未冬定國府征奐夷軍陣亡將士靈位
」﹐地上掃得乾乾淨淨的﹐似乎有人定時來此打掃。
LONG
    );
    set("exits", ([
        "out" : __DIR__"grave5",
    ]));
    set("objects", ([
        __DIR__"npc/dirty_taoist" : 1,
    ]));
    set("map/area", "亂葬崗");
    setup();
    replace_program(ROOM);
}
