// /custom/legacy_npc/jing_yu_tao.c — 舊站 NPC：景愈濤（Jing yu tao）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
舊站區域：古劍門；房間：書房。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("景愈濤", ({ "jing yu tao", "jing", "tao" }));
    set("title", "古劍門掌門");
    set_race("yaksa");
    set_class("fighter");
    set_level(70);
    set("gender", "male");
    set("age", 45);
    set("long",
        "景愈濤，一個頻繁出現在武林傳奇中的人物。三十年前大破冷梅劍嵐\n"
        "若刀，二十年前與柳東蘆論劍靈狐頂，十余年前獨闖青邪尋殘蟻 ...\n"
        "你萬萬想不到的是，他竟然是五堂鎮望族景家的三公子，雖然已入中\n"
        "年但是其瀟灑的舉止，讓人迷戀的風度仍是不減當年。據說，冷梅莊\n"
        "莊主梅影風的夫人是他的義妹，而他當年也正是為了他妹妹才在梅影\n"
        "風和葉風兩人面前一顯身手。\n");
    set("custom_faction", "fighter.au-chan");   // 古劍門（docs/martial_arts/門派武功對照表.md）
    set_power("S");
    setup();
    carry_object("/custom/armor/finger/white_ring")->wear();   // 白玉戒指
    carry_object("/custom/armor/feet/boots_of_cleanse")->wear();   // 塵不沾
    carry_object("/custom/armor/cloth/silver_cloth")->wear();   // 銀絲戰袍
    carry_object("/custom/armor/waist/silver_girth")->wear();   // 銀彎束腰
    carry_object("/custom/armor/armor/white_dragon_plate")->wear();   // 混天白龍
    carry_object("/custom/weapon/sword/flurry_sword")->wield();   // 古岳風刃
}
