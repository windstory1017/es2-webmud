#include <ansi.h>
#include "/daemon/misc/heat_b.h"
inherit ITEM;

private int amount = 1;
int query_amount() { return amount; }
void set_amount(int n)
{
    if (n < 0) error("Negative stack amount.\n");
    HEAT_D->invalidate_inventory();
    amount = n;
    set_weight(n * query("base_weight"));
    if (!n) destruct(this_object());
}
void add_amount(int n) { set_amount(amount + n); }
int render_value() { return amount * query("base_value"); }
void setup()
{
    ::setup();
    set("value", (: render_value :));
    set_amount(amount);
}
int query_burning();   // 下方 short() 先用到，宣告在前
// 燃燒中的松柴在清單上（丹爐內、身上、地上）整段顯示亮紅色；look 只看敘述，照一般顏色（使用者指示 #23，2026-10-30）。
varargs string short(int raw)
{
    string str;
    if (raw) str = ::short(raw);
    else str = chinese_number(amount) + query("base_unit") + ::short(raw);
    return query_burning() ? HIR + str + NOR : str;
}
varargs int move(mixed dest, int silent)
{
    object old;
    old = environment();
    if (!::move(dest, silent)) return 0;
    HEAT_D->arrived(this_object(), old);
    return 1;
}


private int burning, phase;
// 松柴敘述（使用者指示 #23，2026-10-30）：兩者主體相同，燃燒中只多火勢；只寫視覺。
private string cold_long()
{
    return "一捆劈好晒乾的松木柴，長短粗細大致相仿，用草繩紮成一束。\n"
        + "木色赤黃，斷面上一圈圈年輪清晰可見，裂縫間凝著點點琥珀色的松脂。\n"
        + "可用 ignite 點燃它，也可在點燃後用 extinguish 熄滅它。\n";
}
private string burning_long()
{
    return "一捆劈好晒乾的松木柴，長短粗細大致相仿，用草繩紮成一束。\n"
        + "木色赤黃的柴身已燃起火來，橘紅的火舌沿著木紋向上舔舐，不時迸出幾點火星。\n"
        + "可用 extinguish 熄滅它。\n";
}
int is_heat_wood() { return 1; }
int query_burning() { return burning; }
int query_burn_phase() { return phase; }
void set_burn_phase(int n) { phase = n; }
void set_burning(int state)
{
    HEAT_D->invalidate_inventory();
    burning = state;
    if (!state) phase = 0;
    if (state) {
        set_name("燃燒中的松柴", ({"burning firewood", "firewood"}));
        set("long", burning_long());
    } else {
        set_name("松柴", ({"firewood"}));
        set("long", cold_long());
    }
    HEAT_D->track_wood(this_object());
    HEAT_D->refresh_holder(environment());
    HEAT_D->refresh_furnace(environment());
}
void create()
{
    seteuid(getuid());
    set("unit", "捆"); set("base_unit", "根");
    set("base_weight", 120); set("base_value", 5);
    set_burning(0);
    setup();
}
void init()
{
    object me;
    me = this_player();
    if (!me || (environment() != me && environment() != environment(me))) return;
    add_action("do_ignite", "ignite");
    add_action("do_extinguish", "extinguish");
}
int do_ignite(string arg)
{
    if (arg != "firewood" && arg != "burning firewood") return 0;
    if (!HEAT_D->select_wood(this_player())) return 0;
    return HEAT_D->enqueue(this_player(), "ignite", arg);
}
int do_extinguish(string arg)
{
    if (arg != "firewood" && arg != "burning firewood") return 0;
    if (!HEAT_D->select_wood(this_player())) return 0;
    return HEAT_D->enqueue(this_player(), "extinguish", arg);
}
// Called by the canonical liquid move/dissolve protocol, no cross-layer scan.
void dissolve_with(object liquid)
{
    if (liquid && liquid->id("water") && liquid->query_volume() > 0
    && environment(liquid) == environment()) HEAT_D->wet(this_object());
}
