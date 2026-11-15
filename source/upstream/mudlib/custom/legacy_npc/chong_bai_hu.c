// /custom/legacy_npc/chong_bai_hu.c — 舊站 NPC：崇白虎（Chong bai-hu）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
舊站區域：八寶樓；房間：鑄刀房。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("崇白虎", ({ "chong bai-hu", "chong", "bai-hu" }));
    set_race("yaksa");
    set_class("fighter");
    set_level(50);
    set("gender", "male");
    set("age", 55);
    set("long",
        "一生都在追求完美的鑄刀技巧，為了能鑄出一把寶刀，不惜千里跋涉到奐族境\n"
        "內偷學手藝。\n");
    set("custom_faction", "fighter.tiger");   // 虎刀門（docs/martial_arts/門派武功對照表.md）
    set("legacy_aggressive", 1);   // 看到玩家就下殺手
    set_power("A");
    setup();
    carry_object("/custom/armor/waist/iron_girth")->wear();   // 精鐵環扣
    carry_object("/custom/zhenwu/obj/steel_boots")->wear();   // 精鋼戰靴
    carry_object("/custom/armor/armor/tiger_plate")->wear();   // 虎嘯旭日鎧
    carry_object("/custom/weapon/blade/blade_of_fire_spirit")->wield();   // 邪刀‧焱靈
}
