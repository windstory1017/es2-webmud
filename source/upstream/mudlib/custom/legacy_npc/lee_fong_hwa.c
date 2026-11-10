// /custom/legacy_npc/lee_fong_hwa.c — 舊站 NPC：利丰華（Lee fong-hwa）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
舊站區域：隱教；房間：練武場。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("利丰華", ({ "lee fong-hwa", "lee", "fong-hwa" }));
    set("title", "隱教聖女");
    set_race("ashura");
    set_class("thief");
    set_level(50);
    set("gender", "female");
    set("age", 25);
    set("long",
        "一個容色十分美艷的女子，身形婀娜，臉上帶著淺淺的微笑。\n");
    set("custom_faction", "thief.yin");   // 隱教（docs/martial_arts/門派武功對照表.md）
    set_power("A");
    setup();
    carry_object("/custom/armor/cloth/silk_cloth")->wear();   // 天蠶寶衣
    carry_object("/custom/armor/waist/silver_girth")->wear();   // 銀彎束腰
    carry_object("/custom/armor/hand/silk_gloves")->wear();   // 蠶絲手套
    carry_object("/custom/armor/armor/white_dragon_plate")->wear();   // 混天白龍
    carry_object("/custom/armor/finger/white_ring")->wear();   // 白玉戒指
}
