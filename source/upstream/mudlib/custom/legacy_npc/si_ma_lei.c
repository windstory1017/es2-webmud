// /custom/legacy_npc/si_ma_lei.c — 舊站 NPC：司馬雷（Si-ma lei）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
舊站區域：八寶樓；房間：—。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("司馬雷", ({ "si-ma lei", "si-ma", "lei" }));
    set_race("ashura");
    set_class("soldier");
    set_level(60);
    set("gender", "male");
    set("age", 55);
    set("long",
        "大漠八魔中的老七，據說原本是天朝的一名參將，不知為何放著好好的官不做，\n"
        "卻到北方大漠去幹打家劫舍的事。\n");
    set("legacy_aggressive", 1);   // 看到玩家就下殺手
    set_power("A");
    setup();
    carry_object("/custom/weapon/pike/royal_pike")->wield();   // 霸王槍
    carry_object("/custom/armor/head/royal_helmet")->wear();   // 霸王銅冠
    carry_object("/custom/armor/armor/royal_armor")->wear();   // 霸王鎧
    carry_object("/custom/armor/finger/ring_of_mighty_lord")->wear();   // 霸王指環
}
