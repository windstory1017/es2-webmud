// /d/graveyard/obj/urn.c — 骨灰罈（亂葬崗祠堂的邋遢道士身上，依亂葬崗設計表）。
inherit ITEM;

void create()
{
    set_name("骨灰罈", ({ "urn" }));
    set_weight(1500);
    if( !clonep() ) {
        set("unit", "個");
        set("value", 50);
        set("long", "一個瓦罈﹐用來裝骨灰用。\n");
    }
    setup();
}
