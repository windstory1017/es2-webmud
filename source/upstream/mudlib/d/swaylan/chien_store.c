// /d/swaylan/chien_store.c — 水嵐縣（W18）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit ROOM;

void create()
{
    set("short", "錢記商號");
    set("long", @LONG
錢記商號的門面寬敞，貨架上擺滿了青銅丹爐、丹鼎和成捆的松柴，櫃上幾只瓷罐裡盛著山豬果與各色丹藥，連野山蔘都有。老闆生意做得精明，對鄉里卻向來大方，在水嵐縣頗有人望。
LONG
    );
    set("exits", ([
        "north" : __DIR__"east_street1",
    ]));
    set("objects", ([
        __DIR__"npc/richman" : 1,
    ]));
    set("map/area", "水嵐縣");
    setup();
    replace_program(ROOM);
}
