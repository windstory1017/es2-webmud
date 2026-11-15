// /d/mengjia/npc/tomb_guard.c — 孟家莊 NPC：守墓人（F6 墓室）。使用雷法。

inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("守墓人", ({ "tomb guard", "guard" }));
    set_race("human");
    set_class("taoist");
    set_level(50);
    set("gender", "male");
    set("age", 80);
    set("long", "一個長髮披肩，半人半鬼的老人，雖然已經瘦的不成樣子，但一雙鷹眼閃閃發光，一看就知道內力渾厚無比。只是不知他為何會將自己與死人一起關在這個古墓裏。\n");
    set_skill("taoism-thunder", 150);
    set_skill("sword", 100);
    set_skill("dodge", 100);
    set_skill("parry", 100);
    set_power("A");
    setup();
    carry_object("/custom/armor/cloth/exorcist_cassock")->wear();   // 伏魔法衣
    carry_object("/custom/armor/armor/side_armor")->wear();   // 肩鎧
    carry_object("/custom/weapon/sword/peachwood_sword")->wield();   // 桃木劍
    carry_money("coin", 5);   // 五文錢
}
