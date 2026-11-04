// /d/graveyard/npc/young_bonze.c — 亂葬崗 NPC：小和尚（Young bonze），跟著老和尚誦經（依亂葬崗設計表）。
inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("小和尚", ({ "young bonze", "bonze" }));
    set_race("human");
    set_class("monk");
    set_level(1);
    set("gender", "male");
    set("age", 8);
    set("long", @LONG
一個年紀幼小的小和尚，剃得光溜溜的腦袋圓滾滾的，一身僧衣顯得太大，袖口捲了好幾摺。他跟在老和尚身邊有模有樣地敲著木魚，眼睛卻忍不住偷偷往四下的墳堆瞄。
LONG
    );
    set_power("C");
    setup();
    carry_object("/d/graveyard/obj/rosary")->wear();
}
