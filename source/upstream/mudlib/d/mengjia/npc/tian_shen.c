// /d/mengjia/npc/tian_shen.c — 孟家莊 NPC：田申（D8 客棧二樓）。塞北雙熊的老大。

inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("田申", ({ "tian shen", "tian", "shen" }));
    set("title", "塞北雙熊");
    set_race("headless");
    set_class("fighter");
    set_level(30);
    set("gender", "male");
    set("age", 36);
    set("long", "田申是塞北雙熊中的老大，長得高高大大，打人全靠一身蠻力。自從弟弟田由迷戀孟憶柔起，他就待在這裡等待時機。\n");
    set_power("B");
    setup();
    carry_object("/custom/armor/feet/cloud_boots")->wear();   // 流雲靴
    carry_object("/custom/armor/leg/star_studded_legs")->wear();   // 星雲腿護
    carry_object("/custom/weapon/sword/copper_sword")->wield();   // 古銅重劍
}
