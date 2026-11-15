// /custom/legacy_npc/meng_jiang_hong.c — 舊站 NPC：孟驚鴻（Meng jiang-hong）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。已放入孟家莊（d/mengjia/manor.c）；依使用者設計表改為武者 35 級、強度 A（2026-11-14）。
舊站區域：孟家祖墳；房間：—。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("孟驚鴻", ({ "meng jiang-hong", "meng", "jiang-hong" }));
    set("title", "出塵刀");
    set_race("human");
    set_class("fighter");
    set_level(35);
    set("gender", "male");
    set("age", 25);
    set("long",
        "孟家莊的二莊主，一手五虎斷門刀隨然不如其兄孟昭展，但在江湖上\n"
        "也是小有名氣。尤其是在他一刀擊敗塞北雙熊之後，更是獲得了出塵\n"
        "刀的美譽。\n");
    // 孟家祖傳五虎斷門刀 160 級（使用者指示，2026-11-14）；先設技能再 setup()，才不會被自動配置蓋掉。
    set_skill("blade", 160);
    set_skill("five-tiger blade", 160);
    map_skill("blade", "five-tiger blade");
    set_skill("dodge", 120);
    set_skill("parry", 120);
    set_power("A");
    setup();
    carry_object("/custom/armor/cloth/iron_cloth")->wear();   // 鐵布衫
    carry_object("/custom/armor/waist/lion_belt")->wear();   // 獅蠻帶
    carry_object("/custom/armor/leg/red_dust_legs")->wear();   // 風塵腿護
    carry_object("/custom/weapon/blade/ghosts_blade")->wield();   // 百鬼刀
}
