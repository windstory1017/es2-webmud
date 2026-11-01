

#include <ansi.h>

inherit F_CLEAN_UP;

int main(object me, string arg)
{
    int i;
    object *inv, ob;

    if( wizardp(me) && arg ) {
        // 先找同一格裡的對象；遠端只允許查玩家（find_player），
        // 不再用 find_living，避免在任何地方都能看到 NPC（如過路客）身上的東西。
        ob = present(arg, environment(me));
        if( !ob ) ob = find_player(arg);
        if( !ob ) return notify_fail("你要看誰的物品？\n");
    }
    else ob = me;

    inv = all_inventory(ob);
    if( !sizeof(inv) ) {
        write((ob==me)? "目前" + gender_self(me) + "身上沒有任何東西。\n"
            : ob->name() + "身上沒有攜帶任何東西。\n");
        return 1;
    }
    printf("%s身上帶著下列這些東西(負重 %d%%)﹕\n%s\n",
        (ob==me)? gender_self(me) : ob->name(),
        (int)ob->query_encumbrance() * 100 / (1+(int)ob->query_strength("carriage")),
        implode(map_array(inv, "inventory_desc", this_object()), "\n") );

    return 1;
}

string inventory_desc(object ob)
{
    string desc = ob->short();

    if( ob->query("keep_this")) desc += "◎";
//    else desc = "  " + desc;

    // 已裝備以全形「Ｖ」標示，沒有裝備時是一個全形空白，兩者同寬。
    if( ob->query("equipped"))  desc = HIY "Ｖ" + desc + NOR;
    else desc = "　" + desc;

    return desc;
}

int help (object me)
{
    write(@HELP
指令格式: inventory
 
可列出你(妳)目前身上所攜帶的所有物品。
Ｖ 記號及亮黃色表示已在使用， ◎ 記號表示已 keep。
註 : 此指令可以 " i " 代替。
 
HELP
    );
    return 1;
}

