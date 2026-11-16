// /d/qiaoyin_east/npc/stone_stake.c — 白雲觀練武場：石假人（依使用者設計表，2026-11-16）。
// 繼承共用假人基底 /feature/npc/dummy（docs/NPC_DUMMY.md）：不還手、不損壞。
#include <ansi.h>
inherit "/feature/npc/dummy";

void create()
{
    set_name("石假人", ({ "stone stake", "stake", "石假人", "假人" }));
    set_race("human");
    set_class("commoner");
    set_level(20);
    set("long", "一具石頭雕成的人形假人，肩頭與胸口佈滿深淺不一的拳痕與劍痕，石面被磨得光亮。它一動也不動，任憑人拳打腳踢也不會還手，白雲觀的道士常拿它來磨練招式。\n");
    setup();
}
