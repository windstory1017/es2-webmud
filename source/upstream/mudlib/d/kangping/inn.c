// /d/kangping/inn.c — 康平村（J4）。依使用者設計表（2026-10-20）。

#include <room.h>

inherit INN;

void create()
{
    set("short", "小客店");
    set("long", @LONG
你現在來到一家小客店，這家客店很小，連招牌也沒有，牆壁都是木板，風吹過就一陣吱吱嘎嘎響，客店門口一條小路往西。
LONG
    );
    set("exits", ([
        "west" : __DIR__"valley_mouth",
    ]));
    set("objects", ([
        "/custom/legacy_npc/ou_yan_wu_ji_2" : 1,
        "/custom/legacy_npc/liuh_chuen_fon" : 1,
        __DIR__"npc/innkeeper" : 1,
    ]));
    set("no_fight", 1);
    set("valid_startroom", 1);
    set("map/area", "康平村");
    setup();
}
