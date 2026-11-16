// /custom/legacy_npc/lay_ton.c — 舊站 NPC：雷同（Lay ton）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。
2026-11-16 放入白雲觀經堂（d/qiaoyin_east/sutra_hall.c），在白雲觀內巡邏。
舊站區域：斐縣；房間：—。
門派「武陀灸堂」尚未列入門派武功對照表，先不設門派記號。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("雷同", ({ "lay ton", "lay", "ton" }));
    set_race("human");
    set_class("alchemist");
    set_level(20);
    set("gender", "male");
    set("age", 35);
    set("long",
        "雷同乃是斐縣人氏, 曾在武道通門下學了幾年的灸術, 從小好\n"
        "四處旅居增廣見識, 不耐習灸之枯燥乏味, 在灸術有些許火候\n"
        "之後便四處流浪, 近日才又回到故鄉。\n");
    set("legacy_patrol", 1);   // 在白雲觀內巡邏
    set_power("B");
    setup();
    carry_object("/obj/cloth")->wear();   // 布衣
    carry_object("/custom/weapon/dagger/shadow_dagger")->wield();   // 影匕
    carry_object("/custom/weapon/needle/iron_needle")->wield("secondhand needle");   // 透骨釘
}
