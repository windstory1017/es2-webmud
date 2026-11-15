// /custom/legacy_npc/xun_yuan_jian_yue.c — 舊站 NPC：軒轅劍月（Xun-yuan jian yue）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
舊站區域：八寶樓；房間：頂樓。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("軒轅劍月", ({ "xun-yuan jian yue", "xun-yuan", "yue" }));
    set("nickname", "劍皇");
    set_race("human");
    set_class("fighter");
    set_level(65);
    set("gender", "male");
    set("age", 60);
    set("long",
        "此人長的英俊瀟灑，氣度雍容華貴，渾身上下透著一種讓人無法抗拒的魅\n"
        "力。與他的七位結義兄弟不同的是，軒轅劍月很少殺人。\n");
    set("custom_faction", "fighter.snow");   // 雪吟莊（docs/martial_arts/門派武功對照表.md）
    set_skill("six-dao-sword", 195);   // 六陰絕識劍（左手劍法，設計見 docs/martial_arts/左手劍法_六陰絕識劍.md）
    map_skill("secondhand sword", "six-dao-sword");
    set_power("S");
    setup();
    carry_object("/obj/area/obj/longsword")->wield();   // 長劍（右手）
    carry_object("/obj/area/obj/shortsword")->wield("secondhand sword");   // 短劍（左手）
    carry_object("/custom/armor/finger/dragon_ring")->wear();   // 龍吟指環
    carry_object("/custom/armor/feet/flying_boots")->wear();   // 飛天靴
    carry_object("/custom/armor/cloth/blood_stained_cloth")->wear();   // 征衣「血染」
}
