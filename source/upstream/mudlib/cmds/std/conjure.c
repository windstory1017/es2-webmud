// conjure.c

#include <skill.h>

inherit F_CLEAN_UP;

int main(object me, string arg)
{
	string magic, spl, trg;
	object target;
	

	if( me->is_busy() )
		return notify_fail("你現在沒有空﹗\n");

	if( environment(me)->query("no_magic") )
		return notify_fail("這裡不准施展法術。\n");

	if( !arg ) return notify_fail("指令格式﹕conjure <法術> [on <目標>]\n");
	if( sscanf(arg, "%s on %s", spl, trg)==2 ) {
		// 先找身上、再找地上：地上有同名物品時，才不會搶走身上的目標
		// （例如銀咒只能轉身上的東西，#32，2026-11-04）。
		target = present(trg, me);
		if( !target ) target = present(trg, environment(me));
		if( !target ) return notify_fail("這裡沒有 " + trg + "。\n");
	} else {
		spl = arg;
		target = 0;
	}

	spl = replace_string( spl, " ", "_");

	notify_fail("你請先用 enable 指令選擇你要使用的法術系。\n");
	if( stringp(magic = me->skill_mapped("magic")) ) {
		int ok, spi;
		ok = (int)SKILL_D(magic)->conjure_magic(me, spl, target);
		// 施法成功：法術修為得 random(靈性/6) + 1（使用者定案，2026-10-16）；
		// 丹道修養只有施展丹道【神農之術】、丹道【黃白之術】時才給
		// （使用者指示，2026-10-21；黃白之術 2026-10-22 加入）。
		// 法術失敗時，技能檔會設 conjure/failed，這次不加分（使用者定案，2026-10-22）。
		if( ok && userp(me) && !me->query_temp("conjure/failed") ) {
			spi = me->query_attr("spi");
			me->gain_score("magic", (spi >= 6 ? random(spi / 6) : 0) + 1);
			if( magic == "alchemy-medication" || magic == "alchemy-wealth" )
				me->gain_score("alchemy", (spi >= 6 ? random(spi / 6) : 0) + 1);
		}
		// 施法後延遲一次心跳（使用者指示 #37，2026-11-06）：只有真的施出去（成功或失敗）才延遲，
		// 施法前被擋下的不延遲；技能自己設的延遲比較長時照技能的。
		// 唯一例外：神農之術（七色印）施法成功不延遲，失敗照樣延遲。
		if( ok && !(magic == "alchemy-medication" && !me->query_temp("conjure/failed"))
		&&  !me->is_busy() )
			me->start_busy(1);
		me->delete_temp("conjure/failed");
		return ok;
	}
	return 0;
}

int help (object me)
{
        write(@HELP
指令格式﹕conjure <法術名稱> [on <對象>]
 
施法﹐你必需要指定<法術名稱>﹐<施咒對象>則可有可無。
在你使用某一個法術之前﹐你必須先用 enable 指令來指定
你要使用的法術系。施法之後會稍有停頓。
 
HELP
        );
        return 1;
}
