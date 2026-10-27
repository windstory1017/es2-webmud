/*---
description: 茅山法術（mao-shan magic）。茅山派（幻霧觀、隱風觀、靈雲觀）共通法術，enable 在法術（magic）上，
             以 conjure 施展：blank／yin／yang／nature 轉換符紙，remedy、vision 消耗先天符。
             設計見 docs/martial_arts/茅山_法術.md。
---*/
#include <ansi.h>

inherit SKILL;

#define SCROLL "/custom/item/scroll/"

string type() { return "magic"; }
int valid_enable(string usage) { return usage == "magic"; }

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("mao-shan magic");
    setup();
}

/* 學習條件：茅山派（幻霧觀、隱風觀、靈雲觀）。 */
int is_maoshan(object me)
{
    return member_array(me->query("custom_faction"),
        ({ "taoist.fog", "taoist.wind", "taoist.lingyun" })) >= 0;
}

int valid_learn(object me)
{
    if( !is_maoshan(me) )
        return notify_fail("你不是茅山弟子﹐無法修練茅山法術。\n");
    return 1;
}

/* 練成 1 級；每升一級精 +1～2、氣 +1～2、神 +3。 */
void skill_advanced(object me, string skill)
{
    if( !userp(me) ) return;
    me->advance_stat("gin", 1 + random(2));
    me->advance_stat("kee", 1 + random(2));
    me->advance_stat("sen", 3);
}

/* 施法點數（使用者定案，2026-10-21），每次施展成功各自擲一次：
 * 基本法術：((random(靈性)+1) × (random(靈性)+1) + random(靈性)) × (random(層級)+1)
 * 茅山法術：((random(悟性)+1) × (random(靈性)+1) + random(靈性)) × (random(層級)+1)
 * 層級：blank 1；yin、yang、nature 2；remedy、vision 3。 */
void magic_gain(object me, int tier)
{
    int spi, intel;

    if( !userp(me) ) return;
    spi = me->query_attr("spi");
    intel = me->query_attr("int");
    if( spi < 1 ) spi = 1;
    if( intel < 1 ) intel = 1;
    me->improve_skill_exact("magic",
        ((random(spi) + 1) * (random(spi) + 1) + random(spi)) * (random(tier) + 1));
    me->improve_skill_exact("mao-shan magic",
        ((random(intel) + 1) * (random(spi) + 1) + random(spi)) * (random(tier) + 1));
}

object find_scroll(object me, string kind)
{
    foreach(object ob in all_inventory(me))
        if( base_name(ob) == SCROLL + kind ) return ob;
    return 0;
}

/* 把身上的一張 from 符紙換成一張 to 符紙。 */
void swap_scroll(object me, object from, string to)
{
    object ob;

    from->add_amount(-1);
    ob = new(SCROLL + to);
    ob->set_amount(1);
    ob->move(me);
}

int spend_sen(object me, int cost)
{
    if( me->query_stat("sen") < cost )
        return notify_fail("你的心神不足﹐無法施展茅山法術。\n");
    me->consume_stat("sen", cost);
    return 1;
}

int do_blank(object me)
{
    object ob;

    foreach(string kind in ({ "yin", "yang", "nature" }))
        if( ob = find_scroll(me, kind) ) break;
    if( !ob ) return notify_fail("你身上沒有可以化回空白的符紙。\n");
    if( !spend_sen(me, 5) ) return 0;
    message_vision("$N手掐法訣在符紙上一抹﹐符上的硃砂字跡隨即褪去。\n", me, 0, 1);
    swap_scroll(me, ob, "blank");
    magic_gain(me, 1);
    return 1;
}

int do_draw(object me, string kind)
{
    object ob;
    string name;

    if( !(ob = find_scroll(me, "blank")) ) return notify_fail("你身上沒有空白符紙。\n");
    if( !spend_sen(me, 10) ) return 0;
    name = ([ "yin": "少陰符", "yang": "正陽符", "nature": "先天符" ])[kind];
    message_vision("$N提筆蘸了硃砂在符紙上畫了幾筆﹐一張" + name + "便成了。\n", me, 0, 1);
    swap_scroll(me, ob, kind);
    magic_gain(me, 2);
    return 1;
}

/* remedy：消耗先天符；精、氣、神各回復 1～等級/4（各自擲骰）。等級太低時沒有效果。 */
int do_remedy(object me)
{
    object ob;
    int sk;

    if( !(ob = find_scroll(me, "nature")) ) return notify_fail("你身上沒有先天符。\n");
    sk = me->query_skill("mao-shan magic", 1);
    ob->add_amount(-1);
    if( sk / 4 < 1 ) {
        message_vision("$N點燃一張先天符﹐符紙化作一縷青煙﹐卻什麼事也沒發生。\n", me, 0, 1);
        magic_gain(me, 3);
        return 1;
    }
    foreach(string stat in ({ "gin", "kee", "sen" }))
        me->supplement_stat(stat, 1 + random(sk / 4));
    message_vision("$N點燃一張先天符﹐吸入那縷青煙後精神好了許多。\n", me, 0, 1);
    magic_gain(me, 3);
    return 1;
}

void remove_vision(object me, int bonus)
{
    if( !objectp(me) || !me->query_temp("mao_shan/vision") ) return;
    me->add_temp("apply/awarness", -bonus);
    me->add_temp("apply/vision_of_ghost", -1);
    me->delete_temp("mao_shan/vision");
    tell_object(me, CYN "你眼前那層幽光漸漸淡去。\n" NOR);
}

/* vision：消耗先天符；警覺性 +等級/2、陰陽眼；持續 等級/5 個 tick。效果中不能重複施展。 */
int do_vision(object me)
{
    object ob;
    int sk, bonus;

    if( me->query_temp("mao_shan/vision") ) return notify_fail("你的陰陽眼已經開著了。\n");
    if( !(ob = find_scroll(me, "nature")) ) return notify_fail("你身上沒有先天符。\n");
    sk = me->query_skill("mao-shan magic", 1);
    ob->add_amount(-1);
    bonus = sk / 2;
    me->add_temp("apply/awarness", bonus);
    me->add_temp("apply/vision_of_ghost", 1);
    me->set_temp("mao_shan/vision", 1);
    message_vision("$N將先天符貼在眉心﹐雙眼泛起一層淡淡的幽光。\n", me, 0, 1);
    call_out("remove_vision", (sk / 5 > 0 ? sk / 5 : 1) * 2, me, bonus);
    magic_gain(me, 3);
    return 1;
}

int conjure_magic(object me, string spell, object target)
{
    if( !me->query_skill("mao-shan magic", 1) && !me->query_learn("mao-shan magic") )
        return notify_fail("你還沒學會茅山法術。\n");
    switch( spell ) {
    case "blank":  return do_blank(me);
    case "yin":
    case "yang":
    case "nature": return do_draw(me, spell);
    case "remedy": return do_remedy(me);
    case "vision": return do_vision(me);
    default:
        return notify_fail("指令格式﹕conjure <法術名稱> [on <對象>]\n");
    }
}
