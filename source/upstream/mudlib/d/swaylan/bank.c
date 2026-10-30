// /d/swaylan/bank.c — 水嵐縣（S14）。房間敘述依設計表需求撰寫。

#include <room.h>

inherit BANK;

void create()
{
    set("short", "景隆錢莊");
    set("long", @LONG
景隆錢莊在水嵐縣開的分號，門面雖不及總號氣派，櫃檯卻擦得一塵不染，後頭一排烏木錢櫃都上了銅鎖。櫃檯後的算盤聲劈啪不停，來存錢兌銀的客人進進出出，說起話來都壓低了聲音。
LONG
    );
    set("exits", ([
        "south" : __DIR__"west_street",
    ]));
    set("no_fight", 1);
    set("map/area", "水嵐縣");
    setup();
}
