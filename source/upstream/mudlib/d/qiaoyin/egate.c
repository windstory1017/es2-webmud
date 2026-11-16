// /d/qiaoyin/egate.c — 喬陰縣（T10）。依使用者設計表（2026-10-16）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "喬陰縣城東門");
    set("long", @LONG
喬陰縣城的東門，城門緊緊關著，門閂上還加了一道大鎖。出了東門的路原本可以通往黑風山與天寒村，但近來山路不太平靜，官府已經下令暫時封閉，城門邊貼著一張告示，圍觀的鄉民議論紛紛。城牆腳下另有一條小徑繞向東北，看來是附近農戶往來田間的路。
LONG
    );
    set("exits", ([
        "west" : __DIR__"east_street3",
        "northeast" : "/d/qiaoyin_east/outside",
    ]));
    set("map/area", "喬陰縣");
    setup();
    replace_program(ROOM);
}
