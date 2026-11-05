// /d/graveyard/forest_w.c — 亂葬崗（B10）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "密林");
    set("long", @LONG
這一處林子更顯幽深，一株老樹的樹根下鋪著一層乾草，壓得平平整整，旁邊散著幾個空酒罈和啃剩的雞骨，像是有人在此落腳了好些時日。泥地上的腳印步幅極大，落地卻極輕，絕不是尋常的獵戶樵夫留下的。
LONG
    );
    set("outdoors", "snow");
    set("exits", ([
        "east" : __DIR__"forest",
    ]));
    set("objects", ([
        "/custom/legacy_npc/wu_changuei" : 1,
    ]));
    set("map/area", "亂葬崗");
    setup();
    replace_program(ROOM);
}
