/* 龍麟袋 — ES2 參考資料（es2tips）。容量為麻布袋（obj/area/obj/bag.c）的 100 倍。 */

inherit ITEM;

void create()
{
    set_name("龍麟袋", ({ "dragon-skin bag", "dragon skin bag", "bag" }));
    set_weight(1000);
    set_max_encumbrance(3000000);
    if( !clonep() ) {
        set("unit", "隻");
        set("value", 200000);   // 20 兩黃金（1 兩黃金 = 10000 文）
        set("long", "這個大袋子根本就是一張完整的龍麟﹐不怪竟能裝下這麼多東西。\n");
    }
    setup();
}

int accept_object() { return 1; }
