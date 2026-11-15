// /custom/legacy_npc/hua_xing.c — 舊站 NPC：花興（Hua xing）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。已放入孟家莊暗道（d/mengjia/passage.c）；依使用者指示改為盜賊 30 級、強度 A（2026-11-14）。
舊站區域：孟家祖墳；房間：暗道。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("花興", ({ "hua xing", "hua", "xing" }));
    set("nickname", "花蝴蝶");
    set_race("human");
    set_class("thief");
    set_level(30);
    set("gender", "male");
    set("age", 35);
    set("long",
        "江湖中有名的採花大盜，凡是被他遇到的稍有姿色的女子，無一倖免，均是\n"
        "先奸後殺，武林中人全都對他恨之入骨，只是他輕功甚高，多次死裏逃生後\n"
        "便沒了消息，不知為何來孟家莊。\n");
    set_power("A");
    setup();
    carry_object("/custom/weapon/dagger/butterfly_stiletto")->wield();   // 蝴蝶短刺
}
