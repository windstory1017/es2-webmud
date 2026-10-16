// New HEAT B command; canonicalrepo未找到 for prepare.
#include "/daemon/misc/heat_b.h"
int main(object me, string arg)
{
    object ob;
    if (arg != "mixture") return 0;
    foreach (ob in all_inventory(me))
        if (ob->is_heat_mixture()) return HEAT_D->enqueue(me,"prepare",arg);
    if (HEAT_D->has_pending(me)) return HEAT_D->enqueue(me,"prepare",arg);
    return 0;
}
