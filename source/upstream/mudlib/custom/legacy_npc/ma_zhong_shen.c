// /custom/legacy_npc/ma_zhong_shen.c — 舊站 NPC：馬鍾聖（Ma zhong shen）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
舊站區域：八寶樓；房間：拳屋。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("馬鍾聖", ({ "ma zhong shen", "ma", "shen" }));
    set_race("blackteeth");
    set_class("fighter");
    set_level(30);
    set("gender", "male");
    set("age", 55);
    set("long",
        "馬鍾聖在大漠八魔中是老五，為人沈默寡言，很少說話，但是殺起人來卻也是\n"
        "異常殘忍。馬鍾聖在拳法上的造詣極深，由於和他動過手的人都被他殺了，所\n"
        "以他的武功套路幾乎沒人知道。\n");
    set("legacy_aggressive", 1);   // 看到玩家就下殺手
    set_skill("fonxiaoken", 200);   // 風嘯拳法（八寶樓設計表）
    map_skill("unarmed", "fonxiaoken");
    set_power("B");
    setup();
    carry_object("/custom/zhenwu/obj/steel_boots")->wear();   // 精鋼戰靴
    carry_object("/custom/armor/head/iron_helmet")->wear();   // 鐵盔
    carry_object("/custom/armor/hand/steel_bracers")->wear();   // 精鋼護腕
    carry_object("/custom/armor/armor/ring_armor")->wear();   // 連身鎖子甲
}
