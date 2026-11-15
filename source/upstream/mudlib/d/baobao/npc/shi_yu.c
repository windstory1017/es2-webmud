// /d/baobao/npc/shi_yu.c — 「鬼見愁」施鈺（Shi yu），大漠八魔之一，八寶樓幻境。
// 依八寶樓設計表（2026-11-15）：人類、65 歲、武者、30 級、強度 B。
// 舊資料：docs/LEGACY_ROOM_NPC_COMPENDIUM.md〈八寶樓(7) 施鈺 百鬒寒鳩劍〉。
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("施鈺", ({ "shi yu", "shi", "yu" }));
    set("nickname", "鬼見愁");
    set_race("human");
    set_class("fighter");
    set_level(30);
    set("gender", "male");
    set("age", 65);
    set("long",
        "施鈺武功並不高，但不只知從何處學到了極為高深的下毒之術，不論對手有多麼\n"
        "高強的武功都會莫名其妙的中了他下的毒。\n");
    set_power("B");
    setup();
    carry_object("/custom/weapon/sword/hundreds_poison_sword")->wield();   // 百鬒寒鳩劍
    carry_object("/custom/armor/armor/padded_armor")->wear();   // 軟襯甲
    carry_object("/custom/armor/feet/cloud_boots")->wear();   // 流雲靴
}
