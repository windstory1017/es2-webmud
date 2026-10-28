/*---
description: 竹上經心法（zushang）。龍圖丹派方士的內功，安裝在 force
             （enable force with zushang）。exert zushang 打坐修練，
             exert regenerate 消耗精、氣，回復神與受傷減少的神上限。
design: docs/martial_arts/龍圖丹派_竹上經心法.md（2026-10-22）
note: 1～9 級不存在，累積到門檻後 gain 直接練成 10 級。門檻不打折。
      學習條件：龍圖丹派弟子、人物等級 15（使用者指示 #18，2026-10-28），由江述仁傳授前檢查。
---*/

#include <ansi.h>

inherit SKILL;

#define SKILL_ID "zushang"
#define FACTION  "alchemist.dragon"   // 龍圖丹派的門派記號（陳維俠收徒時記上，2026-10-28）

void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon(SKILL_ID);
}

int valid_enable(string usage) { return usage == "force"; }

// 龍圖丹派弟子：有門派記號；或記號補上前就拜師的舊弟子（職業方士、稱號「龍圖丹派弟子」）。
int is_longtu_disciple(object me)
{
    if( me->query("custom_faction") == FACTION ) return 1;
    return me->query_class() == "alchemist" && me->query("title") == "龍圖丹派弟子";
}

// 學習條件（使用者指示 #18，2026-10-28）：龍圖丹派弟子、人物等級 15。
// 傳回 0 時 notify_fail 帶著原因；江述仁傳授前呼叫。
int valid_learn(object me)
{
    if( !is_longtu_disciple(me) )
        return notify_fail(gender_self(me) + "不是龍圖丹派弟子，無法修練竹上經心法。\n");
    if( me->query_level() < 15 )
        return notify_fail(gender_self(me) + "的根基尚淺，恐怕無法瞭解竹上經心法的修練方式。\n");
    return 1;
}

// 練成等級 10；門檻沿用系統預設（10^2 x 100 = 10,000）。
int query_entry_level() { return 10; }

void skill_completed(object me, string sk)
{
    tell_object(me, HIY + gender_self(me) + "覺得一股清涼之氣自丹田緩緩升起，直透靈台，看來"
        + gender_self(me) + "的竹上經心法已經練成了！\n" NOR);
}

// U-01：每升一級精 +1、氣 +1、神 +2，不再受「等級 × 10」上限限制
// （配合伺服器「取消所有內功的精氣神成長上限」，使用者定案，2026-10-23）。不給分數。
void skill_advanced(object me, string sk)
{
    me->advance_stat("gin", 1);
    me->advance_stat("kee", 1);
    me->advance_stat("sen", 2);
}

// Z-01～Z-05：打坐，每次心跳精 1、氣 1、神 3，神的可用上限 -1。
int do_exercise(object me)
{
    if( me->query_stat("gin") < 1
    ||  me->query_stat("kee") < 1
    ||  me->query_stat("sen") < 3 ) {
        tell_object(me, gender_self(me) + "覺得神困力乏，沒有辦法繼續練功了。\n");
        return 0;   // 傳回 0 結束打坐（系統沒有 interrupt_me，舊寫法會一直重複這句）
    }
    me->consume_stat("gin", 1);
    me->consume_stat("kee", 1);
    me->consume_stat("sen", 3);
    me->damage_stat("sen", 1);
    me->improve_skill_exact(SKILL_ID, COMBAT_D->int_formula_exp(me)
        + random(me->query_attr("spi")));
    me->improve_restored_force_tick();
    return 1;
}

int halt_exercise(object me, object owner, object from, string how)
{
    return 1;
}

// 第三章：regenerate。
int do_regenerate(object me)
{
    int level, cost, sen, heal;

    level = me->query_skill(SKILL_ID, 1);
    if( level < 1 ) {
        cost = 1; sen = 10; heal = 5;
    } else {
        cost = level / 6; sen = level; heal = level / 10 + 5;
    }
    if( me->query_stat("gin") < cost || me->query_stat("kee") < cost )
        return notify_fail(gender_self(me) + "的精氣不足，無法運起竹上經心法回復精神。\n");

    me->consume_stat("gin", cost);
    me->consume_stat("kee", cost);
    me->heal_stat("sen", heal);
    me->supplement_stat("sen", sen);
    message_vision("$N運起竹上經心法，臉色微微一白，隨即眼神清明，精神為之一振。\n", me);
    me->start_busy(1);
    return 1;
}

varargs int exert_function(object me, string func, object target)
{
    switch(func) {
    case "zushang":
        message_vision(HIY "$N盤膝而坐，默誦竹上經心法，開始調息運功。\n" NOR, me);
        me->start_busy((: do_exercise, me :), (: halt_exercise, me :));
        return 1;
    case "regenerate":
        return do_regenerate(me);
    default:
        return notify_fail("竹上經心法沒有這種功能。\n");
    }
}
