// /custom/legacy_npc/wu_changuei.c — 舊站 NPC：吳長貴（Wu changuei）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。放在亂葬崗密林（/d/graveyard/forest_w）。
舊站區域：未能判定；房間：—（2026-11-04 放入亂葬崗）。
尚未製作的裝備（先空著）：點鋼匕(Steel dagger)、襯甲(Padded armor)。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("吳長貴", ({ "wu changuei", "wu", "changuei" }));
    set("nickname", "無常鬼");
    set_race("human");
    set_class("thief");
    set_level(15);
    set("gender", "male");
    set("age", 35);
    set("long",
        "一個面皮白淨的高瘦漢子﹐左頰上一道長長的刀疤﹐你突然想起這個\n"
        "漢子就是這一帶著名的飛賊「無常鬼」吳長貴。\n");
    set("custom_faction", "thief.longye");   // 瑯夷派（docs/martial_arts/門派武功對照表.md）
    set_power("B");
    setup();
    carry_object("/obj/area/obj/dagger")->wield();   // 匕首
    carry_object("/custom/armor/head/ghost_mask")->wear();   // 鬼面
    carry_object("/custom/armor/cloth/black_suit")->wear();   // 黑獄服
}
