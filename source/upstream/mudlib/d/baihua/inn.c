// /d/baihua/inn.c — 百花村（N16）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit INN;

void create()
{
    set("short", "紅燈客棧");
    set("long", @LONG
『簷前燈紅滿街照, 門後心酸何足道』. 一個深黑見方的木匾, 兩行金色狂草龍飛鳳舞的題於匾上, 氣勢狂野, 令人眼眩. 偌大一個大廳空空曠曠, 只有幾張板凳孤零零的四散著, 往二樓的樓梯被屏風擋著, 似乎已不宿客了.
LONG
    );
    set("exits", ([
        "west" : __DIR__"road2",
    ]));
    set("objects", ([
        "/custom/legacy_npc/gong_sun_zhi" : 1,
        __DIR__"npc/innkeeper" : 1,
        __DIR__"npc/traveller" : 1,
    ]));
    set("no_fight", 1);
    set("valid_startroom", 1);
    set("map/area", "百花村");
    setup();
}
