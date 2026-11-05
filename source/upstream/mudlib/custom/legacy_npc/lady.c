// /custom/legacy_npc/lady.c — 舊站 NPC：青衣女郎（Lady）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。放在亂葬崗（/d/graveyard/grave5）。
舊站區域：未能判定；房間：亂葬崗。
尚未製作的裝備（先空著）：羊皮短靴(Goatskin buskins)、青布小襖(Green jerkin)。
*/
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("青衣女郎", ({ "lady" }));
    set_race("human");
    set_class("thief");
    set_level(5);
    set("gender", "female");
    set("age", 25);
    set("long",
        "一個二十多歲出頭的青衣女郎﹐容貌 ... 你覺得好像在哪裡看過她 ...\n");
    set_power("B");
    setup();
    carry_object("/obj/area/obj/dagger")->wield();   // 匕首
}
