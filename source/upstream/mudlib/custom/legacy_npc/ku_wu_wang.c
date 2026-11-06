// /custom/legacy_npc/ku_wu_wang.c — 舊站 NPC：哭無望（Ku wu wang）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
舊站區域：雪吟莊；房間：東廂房。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("哭無望", ({ "ku wu wang", "ku", "wang" }));
    set("title", "哭笑門左使");
    set_race("yenhold");
    set_class("fighter");
    set_level(40);
    set("gender", "male");
    set("age", 45);
    set("long",
        "這位滿臉堆笑的肥胖男子就是哭笑門門主劉乙忘玄最得意的兩名弟子中的一\n"
        "位，江湖人稱「哭無望」。至於他的本名自從入哭笑門那日起，就不再曾用\n"
        "過。數年前，劉乙忘玄出關重掌哭笑門，青邪大會之後，江湖之中對其刀法\n"
        "頗多猜忌。也許是為了可以和更多武林名門建立關係，哭無望這次奉劉乙忘\n"
        "玄之命來到雪吟莊。\n");
    set("custom_faction", "fighter.huge");   // 哭笑門（docs/martial_arts/門派武功對照表.md）
    set_power("A");
    setup();
    carry_object("/custom/armor/waist/water_dragon_girdle")->wear();   // 蛟龍帶
    carry_object("/custom/armor/cloth/worn_robe")->wear();   // 舊長袍
    carry_object("/custom/weapon/staff/change_cane")->wield();   // 移殤杖
    carry_object("/custom/armor/neck/silance_rosary")->wear();   // 暗默珠
    carry_object("/custom/armor/head/black_doli")->wear();   // 黑斗笠
}
