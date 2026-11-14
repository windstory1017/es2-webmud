// /d/oldpine/inn.c — 迷霧森林（R14）。房間敘述依設計表提示撰寫。

#include <room.h>
#include <command.h>

inherit INN;

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
    set("short", "野店");
    set("long", @LONG
迷霧森林裡一間用原木搭成的野店，屋簷下掛著一串串獸皮與風乾的鹿腿，門口豎著一根竹竿，挑著寫有「酒」字的布幌子。野店來往的人雖然不多，但獵戶和採藥人經常在此落腳，生意倒也還過得去；偶爾有獵人把打到的獵物賣給野店，烤肉的香味還吸引了附近的老饕前來光顧。往東可以回到林間空地，後頭掛著一塊油膩的布簾，裡面就是廚房。
LONG
    );
    set("detail/布簾", "一塊油膩的布簾，掛在店堂後頭，隱約飄出飯菜的氣味，掀開布簾就能進 (enter) 到廚房去。");
    set("exits", ([
        "east" : __DIR__"glade",
        "enter" : __DIR__"kitchen",
    ]));
    set("objects", ([
        __DIR__"npc/waiter" : 1,
    ]));
    set("map/area", "迷霧森林");
    setup();
}
