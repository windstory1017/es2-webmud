// /d/mengjia/obj/chest.c — 墓室的箱子：固定在房間裡，每次房間重置都補一本【六陰八卦譜】
// （使用者指示，2026-11-14）。做法同 d/shuiyue/obj/tea_pot.c。

inherit ITEM;

#define BOOK "/obj/books/manual_of_six_yin_and_eight_trigrams"

void create()
{
    seteuid(getuid());
    set_name("箱子", ({ "chest", "box" }));
    set_max_encumbrance(30000);
    set("long", "一口蒙著厚厚灰塵的舊木箱，箱蓋半掩著，看起來裡頭放著什麼東西。\n");
    set("no_get", 1);
    setup();
    if( clonep() ) call_out("refill", 0);
}

varargs int accept_object(object me, object ob)
{
    return 1;
}

// 箱子裡沒有六陰八卦譜（被拿走了）就補一本；還在就不重複放。
void refill()
{
    object book;

    if( !objectp(this_object()) || present("manual", this_object()) ) return;
    seteuid(getuid());
    book = new(BOOK);
    if( objectp(book) ) book->move(this_object());
}

void reset()
{
    refill();
}
