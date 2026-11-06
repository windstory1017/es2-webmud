// /custom/legacy_npc/blade_man.c — 舊站 NPC：葬心（Blade man）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
舊站區域：青邪宮、漕幫；房間：酒館二樓。
尚未製作的裝備（先空著）：五行針(Needle of alchemy)。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("葬心", ({ "blade man", "blade", "man" }));
    set("title", "青邪宮禁衛");
    set("nickname", "刀鬼");
    set_race("yenhold");
    set_class("fighter");
    set_level(30);
    set("gender", "male");
    set("age", 35);
    set("long",
        "一個正獨自在喝著酒的灰衣少年，看他帶點憂慮的神情，不知道他碰上了什麼困\n"
        "難，眼光一轉看到他手中把玩的奇特單刀，這種紅色透明的武器不知是用什麼所\n"
        "鑄造成的，如果說這把刀是兵器，不如把它當成藝品還比較恰當。\n");
    set("custom_faction", "fighter.huge");   // 哭笑門（docs/martial_arts/門派武功對照表.md）
    set_power("A");
    setup();
    carry_object("/custom/weapon/blade/blood_blade")->wield();   // 靈刀祭血
    carry_object("/custom/armor/feet/flying_boots")->wear();   // 飛天靴
    carry_object("/custom/armor/cloth/red_cloth")->wear();   // 紅色武道衣
    carry_object("/custom/armor/armor/raw_iron_armor")->wear();   // 生鐵戰甲
}
