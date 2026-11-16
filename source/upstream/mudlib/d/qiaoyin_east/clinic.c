// /d/qiaoyin_east/clinic.c — 喬陰東郊（J16）。依使用者設計表（2026-11-16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "武陀灸堂");
    set("long", @LONG
這裡便是武陀灸堂﹐一陣陣的藥香從裡頭的針房傳了出來。四周的空間並不算大, 只兩張小桌, 一個書櫃, 幾張破椅就差不多塞滿了。四面的牆壁為未經粉刷的土牆, 零落地掛著幾張字畫, 正面的牆上貼著兩張紅聯, 上頭寫著:　　　 　　　　　　　　　　　　 持針濟世道通當仁不讓　　　　　　　　　　　　 　　　　　　　　　　　　 煉丹救民華陀逃之夭夭
LONG
    );
    set("exits", ([
        "north" : __DIR__"hut",
    ]));
    set("objects", ([
        "/custom/legacy_npc/wu_dauton" : 1,
    ]));
    set("map/area", "喬陰東郊");
    setup();
    replace_program(ROOM);
}
