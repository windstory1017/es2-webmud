// /custom/legacy_npc/gong_sun_wei.c — 舊站 NPC：公孫微（Gong-sun wei）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。已放入八寶樓鬼境。
舊站區域：八寶樓；房間：鬼境。
*/
inherit "/custom/legacy_npc/legacy_npc";

// 在八寶樓鬼境的房間清單裡，他是「鬼鬼祟祟地躲在暗處」（舊資料）。
varargs string short(int raw)
{
    string str = ::short(raw);

    return raw ? str : str + " ... 鬼鬼祟祟地躲在暗處";
}

void create()
{
    seteuid(getuid());
    set_name("公孫微", ({ "gong-sun wei", "gong-sun", "wei" }));
    set("nickname", "神行無蹤");
    set_race("jiaojao");
    set_class("thief");
    set_level(45);
    set("gender", "male");
    set("age", 55);
    set("long",
        "此人長得又高又瘦，皮包著骨頭，好象僵屍一樣。一雙小眼睛炯炯有神，正貪\n"
        "婪的盯著你看。\n");
    set("custom_faction", "thief.longye");   // 瑯夷派；八寶樓設計表：不主動攻擊
    set_power("A");
    setup();
    carry_object("/custom/armor/feet/dark_boots")->wear();   // 闇之寶靴
    carry_object("/custom/armor/hand/dark_bracers")->wear();   // 闇之護手
    carry_object("/custom/weapon/dagger/black_iron_dagger")->wield();   // 黑鋼短刃
}
