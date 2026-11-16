// /d/qiaoyin_east/outside.c — 喬陰東郊（F8）。依使用者設計表（2026-11-16）。

#include <room.h>
#include <command.h>

inherit ROOM;

// enter 當成一般出口（和 east 這類方向一樣）；系統的 enter 指令會先攔下，所以這裡轉給 go。
void init()
{
    ::init();
    add_action("do_enter", "enter");
}

int do_enter(string arg)
{
    if( arg && arg != "" ) return 0;
    return GO_CMD->main(this_player(), "enter");
}

void create()
{
    set("short", "喬陰城外");
    set("long", @LONG
出了城牆，土路兩旁是農戶開墾出來的田埂，幾株老柳歪歪斜斜地立在溝渠邊。北面的坡上露出白雲觀的黃牆灰瓦，山門半掩，門前被往來的香客踩出一圈光禿禿的泥地。空氣裏帶著草木的清氣，遠處不時傳來幾聲犬吠。
LONG
    );
    set("exits", ([
        "enter" : __DIR__"hall",
        "southeast" : __DIR__"mountain_road",
        "southwest" : "/d/qiaoyin/egate",
    ]));
    set("map/area", "喬陰東郊");
    setup();
}
