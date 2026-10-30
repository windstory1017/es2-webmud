/* 青龍白虎斧 — ES2 原始武器資料（名稱、敘述）；數值照水嵐縣設計表。密林大統領拿。 */
#include <ansi.h>
#include <weapon.h>

inherit F_AXE;

void create()
{
    set_name("\x1b[1;34m青龍\x1b[1;37m白虎斧\x1b[m", ({ "dragon-tiger axe", "axe" }));
    set_weight(10000);   // 重量同斧頭
    init_damage(3, 8, 60, 9, "axe");
    init_damage(3, 4, 60, 9, "secondhand axe");

    if( !clonep() ) {
        set("wield_as", ({ "axe", "secondhand axe" }));
        set("unit", "把");
        set("value", 30000);
        set("long",
            "一柄相當巨大的大斧﹐不斷的發出嗡嗡的聲響﹐好像對血的飢渴。\n");
        // 數值改照水嵐縣設計表（大統領拿；使用者指示，2026-10-30：新數值、舊敘述）：可單手或左手握，膂力 +1、傷害力 10。
        set("apply_weapon/axe", ([
            "str": 1,
            "damage": 10,
        ]));
        set("apply_weapon/secondhand axe", ([
            "str": 1,
            "damage": 10,
        ]));
    }
    setup();
}
