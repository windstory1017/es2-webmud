// /custom/legacy_npc/huang.c — 舊站 NPC：黃十四娘（Huang）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
舊站區域：隱教；房間：山洞峭壁前。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("黃十四娘", ({ "huang" }));
    set("title", "隱教殺手");
    set_race("human");
    set_class("thief");
    set_level(40);
    set("gender", "female");
    set("age", 25);
    set("long",
        "一個膚色黝黑的女子，年紀似乎不到三十，但是卻滿頭白髮。\n");
    set("custom_faction", "thief.yin");   // 隱教（docs/martial_arts/門派武功對照表.md）
    set_power("B");
    setup();
    carry_object("/custom/armor/feet/soft_leather_boots")->wear();   // 軟皮靴
    carry_object("/custom/armor/armor/shapeless_heartguard")->wear();   // 無形心鏡
    carry_object("/custom/armor/cloth/black_robe")->wear();   // 黑袍
    carry_object("/custom/weapon/dagger/dire_spike")->wield();   // 椎心刺
    carry_object("/custom/armor/neck/dark_necklace")->wear();   // 闇之項鏈
}
