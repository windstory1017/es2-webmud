// /custom/legacy_npc/ling_yue_heng.c — 舊站 NPC：凌月蘅（Ling yue heng）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
舊站區域：未能判定；房間：花砌居。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("凌月蘅", ({ "ling yue heng", "ling", "heng" }));
    set("title", "武學才女");
    set_race("human");
    set_class("fighter");
    set_level(35);
    set("gender", "female");
    set("age", 25);
    set("long",
        "銅鏡前正在梳髮的女子，妝雖清雅，然而依舊能讓你有驚為天人\n"
        "的氣質。黑亮長髮垂至腰擺，細白的肌膚微透著紅暈。像極了古\n"
        "畫中讚歎不已的蘇州美人。別見她柔若無骨的姿態，她也是自幼\n"
        "跟從其父凌承軒習武的，一身武藝毫不遜色。從銅鏡看她，眼神\n"
        "若有所思的停佇在手中的那隻簪，大概是想念她的夫君蔚楓吧。\n");
    set("custom_faction", "fighter.au-chan");   // 古劍派（docs/martial_arts/門派武功對照表.md）
    set_power("A");
    setup();
    carry_object("/custom/weapon/dagger/wolf_dagger")->wield();   // 狼匕
    carry_object("/custom/weapon/dagger/beauty_dagger")->wield("secondhand dagger");   // 七彩琉璃匕
    carry_object("/custom/armor/feet/embroidery_boots")->wear();   // 繡花鞋
    carry_object("/custom/armor/waist/girl_girth")->wear();   // 束腰帶
    carry_object("/custom/armor/armor/light_plate")->wear();   // 光鎧
    carry_object("/d/snow/npc/obj/white_dress")->wear();   // 白綢羽衫
}
