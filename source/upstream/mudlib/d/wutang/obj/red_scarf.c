// /d/wutang/obj/red_scarf.c — 小紅巾（蝶影幻步任務道具，設計見 docs/martial_arts/瑯夷派_蝶影幻步.md）。
// 記著是誰拿到的（quest_owner）；別人拿去交給廟祝不算數。價值 100。
inherit ITEM;

void create()
{
    set_name("小紅巾", ({ "red scarf", "scarf", "note" }));
    set_weight(50);
    if( !clonep() ) {
        set("unit", "條");
        set("value", 100);
        set("long", "一條摺得整整齊齊的小紅巾，看起來有些年頭了。\n");
    }
    setup();
}
