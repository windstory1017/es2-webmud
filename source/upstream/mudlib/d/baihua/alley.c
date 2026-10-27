// /d/baihua/alley.c — 百花村（N14）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "小巷");
    set("long", @LONG
你順著小路走, 這兒尋常人雖不多, 路旁的商家倒還挺會作生意的, 不斷向你招呼著. 一陣香肉味, 隱隱傳進你鼻子裡, 你不禁想起有好一陣子沒有吃香喝辣的了, 這就犒賞犒賞自己南北奔波, 終日殺戮的身子吧 !
LONG
    );
    set("exits", ([
        "west" : __DIR__"road1",
    ]));
    set("objects", ([
        __DIR__"npc/san" : 1,
        __DIR__"npc/girl" : 1,
    ]));
    set("outdoors", "baihua");
    set("map/area", "百花村");
    setup();
    replace_program(ROOM);
}
