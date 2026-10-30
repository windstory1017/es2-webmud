// /d/swaylan/buxuan/hut.c — 落雲峰（AO8）。舊資料全文引用（刪一個「的」字對齊）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "小茅屋");
    set("long", @LONG
你現在站在一間小茅屋中，裡面只有一張竹桌跟兩張椅子，在你面前有一個年輕人，身穿白色長袍，手中拿著一把判官筆，正對著竹桌上面的白布作畫著，看神情似乎是很專注，在你後面的門望出去可以看到一些飛鳥在盤旋著，山風從窗戶灌進來讓你覺得有些寒冷的感覺。
LONG
    );
    set("exits", ([
        "south" : __DIR__"cliff_top",
    ]));
    set("objects", ([
        __DIR__"npc/luo_yun_feng" : 1,
    ]));
    set("map/area", "落雲峰");
    setup();
    replace_program(ROOM);
}
