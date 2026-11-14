// /d/fengcheng/npc/hua_zi_yi.c — 風城 NPC：青雲觀主 華子儀（J10 青雲觀）。

inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("華子儀", ({ "hua zi-yi", "hua", "zi-yi" }));
    set("title", "青雲觀主");
    set_race("human");
    set_class("taoist");
    set_level(60);
    set("gender", "male");
    set("age", 60);
    set("long", "青雲觀主華子儀，當世少數掌握天師四破雷咒的佼佼者。早年斬妖除魔、保家衛國，如今在青雲觀潛心修行，鬚髮雖已斑白，眼神卻清亮如電。\n");
    set_skill("taoism-thunder", 140);
    set_skill("sword", 100);
    set_skill("dodge", 100);
    set_skill("parry", 100);
    set_power("S");
    setup();
    carry_object("/custom/weapon/sword/numinous_sword")->wield();   // 虛靈劍
    carry_object("/custom/armor/head/purple_helmet")->wear();   // 紫雲盔
    carry_object("/custom/armor/cloth/purple_taoist_robe")->wear();   // 紫陽道袍
}
