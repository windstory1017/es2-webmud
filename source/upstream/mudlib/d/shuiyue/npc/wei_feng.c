// /d/shuiyue/npc/wei_feng.c — 水月村 NPC：劍領風騷 蔚楓（石屋）。只放 NPC，不拜師。

inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    object sword;

    seteuid(getuid());
    set_name("蔚楓", ({ "wei feng", "wei", "feng" }));
    set("title", "劍領風騷");
    set_race("human");
    set_class("fighter");
    set_level(60);
    set("gender", "male");
    set("age", 30);
    set("long", "蔚楓是古劍派的傳人，如今隱居在這間石屋裡，看上去不過三十出頭，神情淡泊，一身舊布衣漿洗得乾乾淨淨。他早已不問江湖事，眉宇之間卻仍隱隱透著劍客特有的鋒芒。\n");
    set_skill("sword", 140);
    set_skill("dodge", 120);
    set_skill("parry", 120);
    set_skill("force", 120);
    set_power("S");
    setup();
    // 古劍醒塵是唯一的一把；被玩家打走（還在世上）時，蔚楓重生就改拿巨神劍。
    sword = carry_object("/custom/weapon/sword/au_han_sword");   // 古劍醒塵
    if( !objectp(sword) || environment(sword) != this_object() )
        sword = carry_object("/custom/weapon/sword/colosus_sword");   // 巨神劍
    if( objectp(sword) ) sword->wield();
    carry_object("/custom/armor/cloth/iron_cloth")->wear();   // 鐵布衫
    carry_object("/custom/armor/feet/forgod_boots")->wear();   // 封神靴
}
