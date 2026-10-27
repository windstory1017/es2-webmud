

#include <ansi.h>

inherit F_CLEAN_UP;

// NEW（非原始 ES2 內容）：附加值的名稱與 identify 一致，數值類附加不顯示英文代碼。
mapping named_apply = ([
    "attack" : "攻擊能力值",
    "defense" : "防禦能力值",
    "str" : "膂力",
    "cor" : "膽識",
    "cps" : "定力",
    "int" : "悟性",
    "wis" : "慧根",
    "spi" : "靈性",
    "con" : "根骨",
    "dex" : "機敏",
    "damage" : "傷害力",
    "armor" : "防禦力",
    "spell" : "咒文能力",
    "magic_ability" : "魔力",
    "move" : "行動力",
    "damage_vs_fire" : "火焰傷害力",
    "damage_vs_ice" : "冰寒傷害力",
    "damage_vs_lightning" : "雷電傷害力",
    "damage_vs_wind" : "風擊傷害力",
]);

int main(object me, string arg)
{
    object ob;
    string str;
    mapping apply, weapon;

    // 玩家只能分析自己；巫師可以分析任何人（使用者指定開放給玩家，2026-10-22）。
    if( !arg || arg == "me" || me->id(arg) ) ob = me;
    else if( !wizardp(me) )
        return notify_fail("你只能看透自己的能耐，別人的底細可沒那麼容易摸清。\n");
    else {
        if( !(ob = find_player(arg))
        &&    !(ob = present(arg, environment(me)))
        &&    !(ob = find_living(arg)) )
            return notify_fail("這裡沒有 " + arg + " 這種生物。\n");
    }

    str = sprintf("%s的各項能力分析﹕\n", ob->name());
    str += HIY "\n<基本值 - 來自屬性>\n" NOR;
    str += sprintf("攻擊力\t\t攻勢 %d，技巧 %d，力道 %.2f 公斤\n",
	ob->query_ability("intimidate"),
	ob->query_ability("attack"),
	ob->query_strength("attack") / 1000.0);
    str += sprintf("防禦力\t\t守勢 %d，技巧 %d，強度 %.2f 公斤\n",
	ob->query_ability("wittiness"),
	ob->query_ability("defense"),
        ob->query_strength("defense") / 1000.0);
    str += sprintf("魔力\t\t能力值 %d﹐強度 %.2f Kw\n", ob->query_ability("magic"),
        ob->query_strength("magic") / 1000.0);
    str += sprintf("法力\t\t能力值 %d﹐強度 %.2f Kw\n", ob->query_ability("spell"),
        ob->query_strength("spell") / 1000.0);

    str += HIY "\n<修正值 - 來自技能>\n" NOR;
    if( ob->query_skill("force") && ob->skill_mapped("force")!="force" ) {
        int skill, modify;
        skill = ob->query_skill("force");
        modify = skill * ob->query_stat("kee") * 4;
        str += sprintf("使用內力 \t%s(攻擊強度 +%.2f Kg)\n",
            to_chinese(ob->skill_mapped("force")), (float)modify/1000.0);
    }
    if( mapp(weapon  = ob->query_temp("weapon")) && sizeof(weapon) ) {
        string term;
        str += "使用武器\t";
        foreach(term in keys(weapon)) {
        str += sprintf("%s﹐(攻擊能力 %+d)\n\t\t", weapon[term]->name() + "(" + term + ")",
                    ob->query_skill(term));
        }
    }
    else str += sprintf("徒手攻擊 \t(攻擊能力 %+d)\n", ob->query_skill("unarmed"));

    if( mapp(apply = ob->query_temp("apply")) ) {
        string term;
        int prop, k;

        str += HIY "\n<附加值 - 來自裝備，法術等影響>\n" NOR;
        k = 0;
        foreach(term, prop in apply) {
            if( !intp(prop) ) continue;
            if( prop ) {
                // 用 cjk_align 補齊，中英夾雜的名稱在網頁字型下也能對齊。
                str += sprintf("  %s%-6s%s",
                    cjk_align(undefinedp(named_apply[term]) ? to_chinese(term) : named_apply[term], 10, 2),
                    sprintf("%+d", prop), k%2==0 ? "  " : "\n");
                k++;
            }
        }
    }
    str += "\n";

    write(str);
    return 1;
}

int help()
{
    write(@TEXT
指令格式﹕analyze

這個指令會列出你自己的各項能力值﹐包括攻擊、防禦、魔力、法力﹐以及
技能與裝備帶來的修正。

巫師可以用 analyze <對象> 分析其他人物。
TEXT
    );
    return 1;
}

