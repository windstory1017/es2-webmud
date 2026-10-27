// /d/lee/inn.c
// 李家村復原版。CONFIRMED_MIXED
// Topology follows the user-provided Li Village map; generated prose is intentionally replaceable.

inherit ROOM;

void create()
{
    set("short", "小旅店");
    set("long", @LONG
這是一間規模很小的旅店，屋裡只有幾張老舊桌椅。店小二忙著招呼客人，說書先生李半仙偶爾在此停留，東門通往街道。牆角一張矮桌上擺著粗陶茶壺與幾個缺了口的碗，看得出這裡的客人多是圖個實在，不講究排場。牆上掛著一塊掉漆的木牌，歪歪斜斜地寫著幾樣簡單的吃食。
LONG
    );
    set("objects", ([
        "/d/lee/npc/waiter" : 1,
        "/d/lee/npc/li_ban_xian" : 1,
        "/custom/legacy_npc/du_feng_ling" : 1     // 幻霧觀觀主（使用者指示，2026-10-25）
    ]));
    set("exits", ([
        "east" : __DIR__"street2"
    ]));
    set("no_fight", 1);
    setup();
    replace_program(ROOM);
    set("map/area", "李家村");
}
