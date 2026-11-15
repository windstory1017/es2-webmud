// /custom/legacy_npc/bai_yu.c — 舊站 NPC：白魚（Bai yu）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
舊站區域：八寶樓；房間：魔境。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("白魚", ({ "bai yu", "bai", "yu" }));
    set("nickname", "天神地鬼");
    set_race("malik");
    set_class("taoist");
    set_level(50);
    set("gender", "male");
    set("age", 280);
    set("long",
        "白魚是大漠八魔的老大，但為人優柔寡斷，所以凡事都由老二來主持。\n");
    set("legacy_aggressive", 1);   // 看到玩家就下殺手
    set("custom_faction", "taoist.storm");   // 玄衣派
    set_power("A");
    setup();
    carry_object("/custom/armor/head/copper_hat")->wear();   // 青銅道冠
    carry_object("/custom/armor/cloth/seven_star_robe")->wear();   // 七星道袍
    carry_object("/custom/weapon/whip/angry_whip")->wield();   // 怒天鞭
}
