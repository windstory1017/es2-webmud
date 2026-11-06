// /custom/legacy_npc/xing_long.c — 舊站 NPC：刑龍（Xing long）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
舊站區域：銅佛寺；房間：客房。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("刑龍", ({ "xing long", "xing", "long" }));
    set("title", "催情杖");
    set_race("headless");
    set_class("fighter");
    set_level(35);
    set("gender", "male");
    set("age", 35);
    set("long",
        "一道熾熱的目光掃的你全身不自在了起來，如果不是剛摸了摸身上的衣袍，\n"
        "你真的以為自己忘了穿上衣服了....，轉身順著目光望了過去，一個身材\n"
        "瘦弱的男子，他的臉色蒼白，好像是身染重病一樣。男子揹在身後哪把與\n"
        "其身材不成比例的長杖，詭譎的流線、古怪的造型，正散發著一陣又一陣\n"
        "的淫靡之氣，刺激的你下身火熱.....\n");
    set("custom_faction", "fighter.huge");   // 哭笑門（docs/martial_arts/門派武功對照表.md）
    set_power("A");
    setup();
    carry_object("/custom/armor/feet/cloud_boots")->wear();   // 流雲靴
    carry_object("/custom/armor/waist/water_dragon_girdle")->wear();   // 蛟龍帶
    carry_object("/custom/weapon/staff/staff_of_eroticism")->wield();   // 交歡杖
}
