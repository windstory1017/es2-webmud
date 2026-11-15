// /custom/legacy_npc/o_yan_zhi.c — 舊站 NPC：歐陽芝（O-yan zhi）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。舊站區域：名山書肆、八寶樓；房間：歐陽書屋。
門派步玄派（scholar.myst）、30 歲依八寶樓設計表（2026-11-15）。已放入八寶樓歐陽書房。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("歐陽芝", ({ "o-yan zhi", "o-yan", "zhi" }));
    set("nickname", "神算無疑");
    set_race("human");
    set_class("scholar");
    set_level(30);
    set("gender", "male");
    set("age", 30);
    set("long",
        "大漠八魔之一，武功似乎是步玄派的，但為人心浮氣躁，自視極高，自認為風\n"
        "流倜儻，文采天下無雙。\n");
    set("custom_faction", "scholar.myst");   // 步玄派（docs/martial_arts/門派武功對照表.md）
    set_power("B");
    setup();
    carry_object("/obj/area/obj/leather_boot")->wear();   // 皮短靴
    carry_object("/obj/area/obj/belt")->wear();   // 腰帶
    carry_object("/custom/armor/cloth/white_cloth")->wear();   // 白長袍
    carry_object("/obj/area/obj/longsword")->wield();   // 長劍
    carry_object("/obj/books/strange_tales_of_the_celestial_court");   // 掉落：【天朝異事錄】
}
