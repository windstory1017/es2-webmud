#include "/daemon/misc/heat_b.h"
inherit ITEM;
int is_heat_reactor() { return 1; }

// 神農之術封印（技能設計 01，S-10～S-17）：記錄封印顏色；0＝沒有封印。
private string seal_color;
string query_seal() { return seal_color; }
void set_seal(string color) { seal_color = color; }
// S-17：外觀只顯示有封印，不顯示顏色。
varargs string long(int raw)
{
    string text;
    text = ::long(raw);
    if (!raw && stringp(seal_color)) text += "鼎口封著一道符印。\n";
    return text;
}
void create()
{
    set_name("青銅丹鼎", ({"bronze reactor", "reactor"}));
    set("unit", "座"); set("value", 250);
    set("long", "一座用來盛放配方的青銅丹鼎。\n");
    set_weight(1500); set_max_encumbrance(8000); setup();
}
int mixture_count()
{
    int n;
    object ob;
    foreach (ob in all_inventory())
        if (ob->is_heat_mixture()) n += ob->query_amount();
    return n;
}
int accept_object(object me, object ob)
{
    if (ob->is_heat_mixture() && environment(ob) != this_object()
    && mixture_count() + ob->query_amount() > 5)
        return notify_fail("丹鼎最多只能容納五份配方。\n");
    return 1;
}
int receive_object(object ob, int from_inventory)
{
    if (!accept_object(this_player(), ob)) return 0;
    return ::receive_object(ob, from_inventory);
}

varargs int move(mixed dest, int silent) {
    object ob; object *contents;
    int ok;
    contents=all_inventory();
    foreach(ob in contents) if(ob->is_heat_mixture()) ob->suspend_phase();
    ok=::move(dest,silent);
    HEAT_D->invalidate_inventory();
    foreach(ob in contents) if(ob && ob->is_heat_mixture()) ob->resume_phase();
    return ok;
}
