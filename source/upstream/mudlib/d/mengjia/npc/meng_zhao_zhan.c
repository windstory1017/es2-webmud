// /d/mengjia/npc/meng_zhao_zhan.c — 孟家莊 NPC：「多情刀」孟昭展（F12 大街）。孟家莊莊主，五虎斷門刀。

inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("孟昭展", ({ "meng zhao-zhan", "meng", "zhao-zhan" }));
    set("title", "多情刀");
    set_race("human");
    set_class("fighter");
    set_level(35);
    set("gender", "male");
    set("age", 32);
    set("long", "孟家莊的莊主，年紀輕輕便練成了祖傳的五虎斷門刀，成為江湖中的新勢力。孟昭展不僅武功高強，而且英俊瀟灑，行走江湖期間，迷倒了無數的才女俠女，但他好像已經有了意中人，對其他少女都是以禮相待，他也因此得了個多情刀的綽號。\n");
    // 孟家祖傳五虎斷門刀 160 級（使用者指示，2026-11-14）；先設技能再 setup()，才不會被自動配置蓋掉。
    set_skill("blade", 160);
    set_skill("five-tiger blade", 160);
    map_skill("blade", "five-tiger blade");
    set_skill("dodge", 120);
    set_skill("parry", 120);
    set_power("A");
    setup();
    carry_object("/custom/armor/cloth/white_scholar_dress")->wear();   // 白色長衫
    carry_object("/custom/armor/leg/red_dust_legs")->wear();   // 風塵腿護
    carry_object("/custom/armor/waist/water_dragon_girdle")->wear();   // 蛟龍帶
    carry_object("/custom/weapon/blade/ghosts_blade")->wield();   // 百鬼刀
}
