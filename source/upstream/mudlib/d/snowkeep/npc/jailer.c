// /d/snowkeep/npc/jailer.c — 雪吟莊 NPC：獄卒。守在地牢（H2），攔人往地牢深處（J2）。
// 設定依使用者提供的雪吟莊設計表（2026-11-06）；攔人在房間 d/snowkeep/dungeon.c 的 valid_leave()。

#include <npc.h>
inherit F_FIGHTER;

void create()
{
    seteuid(getuid());
    set_name("獄卒", ({ "jailer" }));
    set_race("human");
    set_class("fighter");
    set_level(50);
    set("gender", "male");
    set("age", 40);
    set("long",
        "這名獄卒身材魁梧，太陽穴高高鼓起，一雙眼睛精光內斂，顯然身負上乘武功，\n"
        "卻不知為何甘願屈身在這陰暗的地牢裏看守牢門。\n");
    set("snowkeep_jailer", 1);
    set_power("A");
    setup();
    carry_object("/custom/weapon/blade/blade_of_hydra_bone")->wield();   // 蛟骨刀
    carry_object("/custom/armor/cloth/force_cloth")->wear();             // 真元戰袍
    carry_object("/custom/armor/leg/sun_legs")->wear();                  // 烈陽護膝
    carry_object("/custom/armor/waist/perl_girth")->wear();              // 鑲珠束腰
}
