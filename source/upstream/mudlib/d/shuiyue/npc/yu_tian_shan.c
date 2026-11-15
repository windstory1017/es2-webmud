// /d/shuiyue/npc/yu_tian_shan.c — 水月村 NPC：「百步穿楊」余天山（木屋內）。
// 製作獵具「缺材料」目前只是人物敘述的暗示，沒有製作功能。

inherit "/custom/legacy_npc/legacy_npc";

void create()
{
    seteuid(getuid());
    set_name("余天山", ({ "yu tian-shan", "yu", "tian-shan" }));
    set("title", "百步穿楊");
    set_race("human");
    set_class("fighter");
    set_level(40);
    set("gender", "male");
    set("age", 60);
    set("long", "余天山是附近有名的獵戶，一手箭術在山裡無人不曉，人稱「百步穿楊」。他年輕時常為鄉親打造弓箭與各式獵具，如今眉間總帶著愁容，望著牆上的舊獵具發呆，嘴裡嘆著手邊缺了好材料，否則還能替人打造幾件趁手的傢伙。\n");
    set_skill("unarmed", 80);
    set_skill("dodge", 80);
    set_skill("parry", 80);
    set_power("B");
    setup();
}
