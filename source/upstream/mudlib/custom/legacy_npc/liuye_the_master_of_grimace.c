// /custom/legacy_npc/liuye_the_master_of_grimace.c — 舊站 NPC：劉乙忘玄（Liuye the Master of Grimace）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
舊站區域：哭笑門、青邪宮；房間：青邪宮哭笑門鎮心。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("劉乙忘玄", ({ "liuye the master of grimace", "liuye", "grimace", "master" }));
    set("title", "哭笑門門主");
    set("nickname", "哭笑野人");
    set_race("yenhold");
    set_class("fighter");
    set_level(70);
    set("gender", "male");
    set("age", 65);
    set("long",
        "這位兩眼如鷹, 滿臉鬚髦的八尺大漢, 正是青邪宮四門中位於首座的哭笑門\n"
        "門主劉乙忘玄。劉乙忘玄是青邪宮殘蟻師太的大弟子, 武藝超群, 為人極為\n"
        "深沉內斂, 年輕時膂力驚人, 身形魁梧, 一手青邪杖法打遍難逢敵手, 在當\n"
        "時在江湖上便得了個「降天山」的外號。\n"
        "劉乙忘玄中年時青邪宮發生重大變故, 青邪宮四門中的擾心門門主梵雲飛,\n"
        "和吹雪門門主王雙兒雙雙失蹤, 當時參與青邪宮「封鬼」盛會的武林豪傑死\n"
        "傷更是慘重。劉乙忘玄在此事過後, 突然性情大變, 整天又哭又笑, 意態狂\n"
        "亂, 狀如瘋癲。五十三歲那年, 殘蟻逝世, 劉乙忘玄瘋病竟然好轉, 自號「\n"
        "哭笑野人」, 成為青邪宮繼殘蟻師太之後的霸主, 之後潛心閉關五年, 創出\n"
        "「大邪心法」和「大邪七損刀」, 儼然成為武林中人皆敬佩的武學大宗師。\n");
    set("custom_faction", "fighter.huge");   // 哭笑門（docs/martial_arts/門派武功對照表.md）
    set_power("S");
    setup();
    carry_object("/custom/armor/feet/flying_boots")->wear();   // 飛天靴
    carry_object("/custom/armor/waist/ghosts_girdle")->wear();   // 百鬼腰束
    carry_object("/custom/weapon/staff/god_cane")->wield();   // 九曲十彎
    carry_object("/custom/armor/cloth/soft_cloth")->wear();   // 軟麻青絲衣
    carry_object("/custom/armor/neck/copper_rosary")->wear();   // 黃銅佛珠
}
