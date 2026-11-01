/*---
description: 茅山奇術【遁甲書】（taoism of dunja）。隱風觀法術，enable 在法術（magic）上，以 conjure 施展。
             fate 天命：之後 conjure <技能> on <玩家> 說出對方這項技能的上限。
             experience 心眼：之後 conjure <技能> on <玩家> 把自己的法術修為渡給對方當技能經驗。
             設計見 docs/martial_arts/隱風觀_茅山幻術與遁甲書.md。
---*/
#include <ansi.h>

inherit SKILL;

#define FACTION "taoist.wind"

string type() { return "magic"; }
int valid_enable(string usage) { return usage == "magic"; }

private void create()
{
    seteuid(getuid());
    DAEMON_D->register_skill_daemon("taoism of dunja");
    setup();
}

/* 學習條件：隱風觀弟子、茅山幻術 90 級、人物等級 30、靈性（不含裝備）30。 */
int valid_learn(object me)
{
    if( me->query("custom_faction") != FACTION )
        return notify_fail("你不是隱風觀弟子﹐無緣翻閱這部遁甲書。\n");
    if( me->query_skill("taoism of nature", 1) < 90 )
        return notify_fail("你的茅山幻術尚未大成﹐翻不開這部遁甲書。\n");
    if( me->query_level() < 30 )
        return notify_fail("你的根基尚淺﹐還參不透遁甲書的玄機。\n");
    if( me->query_attr("spi", 1) < 30 )
        return notify_fail("你的靈性不足﹐悟不透遁甲書裡的天機。\n");
    return 1;
}

/* 練成 20 級。 */
int query_entry_level() { return 20; }

void skill_completed(object me, string skill)
{
    tell_object(me, HIG "你將遁甲書從頭到尾參詳了一遍﹐胸中隱隱已有天地運轉之數。\n" NOR);
}

/* 施法點數（照茅山法術）：
 * 基本法術：((random(靈性)+1) × (random(靈性)+1) + random(靈性)) × (random(層級)+1)
 * 遁甲書：((random(悟性)+1) × (random(靈性)+1) + random(靈性)) × (random(層級)+1)
 * 層級：天命 1、心眼 2。 */
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
    me->improve_skill_exact("taoism of dunja",
        ((random(intel) + 1) * (random(spi) + 1) + random(spi)) * (random(tier) + 1));
}

int spend_sen(object me, int cost)
{
    if( me->query_stat("sen") < cost )
        return notify_fail("你心神渙散﹐無法施展遁甲之術。\n");
    me->consume_stat("sen", cost);
    return 1;
}

/* 天命：耗神 20、停頓 1 回合；保持到算一次命為止。 */
int do_fate(object me)
{
    if( me->query_temp("dunja/fate") )
        return notify_fail("你已推算過天命﹐眼中清光猶在。\n");
    if( !spend_sen(me, 20) ) return 0;
    me->delete_temp("dunja/experience");
    me->set_temp("dunja/fate", 1);
    message_vision("$N盤膝而坐屈指推算天干地支﹐眼中隱隱透出一點清光。\n", me, 0, 1);
    me->start_busy(1);
    magic_gain(me, 1);
    return 1;
}

/* 心眼：耗神 30、停頓 1 回合；保持到渡一次經驗為止。心眼中可以再施展（重新開啟，照樣耗神）。 */
int do_experience(object me)
{
    if( !spend_sen(me, 30) ) return 0;
    me->delete_temp("dunja/fate");
    me->set_temp("dunja/experience", 1);
    message_vision("$N閉目凝神﹐眉心彷彿睜開了第三隻眼﹐周遭一切都看得通透。\n", me, 0, 1);
    me->start_busy(1);
    magic_gain(me, 2);
    return 1;
}

/* 天命的批語（使用者提供，2026-10-22），依技能上限。
 * 批語是當面說給對方聽的，稱呼一律用對方職業的尊稱（道長、英雄……，2026-10-23）。 */
string fate_words(object target, string skill, int cap)
{
    string sk, who, title;

    sk = to_chinese(skill);
    who = title = target->rank("respectful");
    if( cap >= 200 )
        return sk + "發展可說是無可限量，舉世無匹！老夫行卜多年，以" + who + "的潛力最為驚人！"
            + who + "若是能在此技深入鑽研，日後定能成為一代宗師！";
    if( cap >= 190 )
        return sk + "發展委實是非同凡響！依" + who + "在此技上的天份，只要假以時日，必能練就震動天下的驚人藝業！";
    if( cap >= 180 )
        return sk + "發展非常不一般呀！看來" + title + "日後定然能以此技笑傲江湖！";
    if( cap >= 170 )
        return sk + "發展相當不錯呀！看來" + title + "可以憑此技名揚天下！";
    if( cap >= 160 )
        return sk + "發展不錯，若是棄而不練，真是可惜啊。";
    if( cap >= 150 )
        return sk + "發展還算不錯，只要肯下苦功，他日當有一番成就。";
    return sk + "發展一般，雖不能成名，但也可憑此謀生。";
}

/* 天命狀態下：悄悄告訴對方這項技能的上限，用掉天命。 */
int read_fate(object me, object target, string skill)
{
    string msg;

    msg = fate_words(target, skill, target->query_personal_skill_cap(skill));
    me->delete_temp("dunja/fate");
    tell_object(me, GRN "你在" + target->name() + "的耳邊悄聲說道﹕" + msg + "\n" NOR);
    tell_object(target, GRN + me->name() + "在你的耳邊悄聲說道﹕" + msg + "\n" NOR);
    tell_room(environment(me), me->name() + "在" + target->name() + "耳邊小聲地說了些話。\n",
        ({ me, target }));
    return 1;
}

/* 心眼狀態下：把法術修為渡給對方當技能經驗，用掉心眼；施法者停頓 1 回合。
 * 經驗 (random(慧根)+1) × (random(靈性)+1) × random(遁甲書/20)；消耗修為 = 經驗 ÷ 10。 */
int give_experience(object me, object target, string skill)
{
    int wis, spi, gain;

    /* 遁甲書 60 級起才渡得出經驗（使用者定案，2026-10-29）；不夠時不用掉心眼。 */
    if( me->query_skill("taoism of dunja", 1) < 60 )
        return notify_fail("你的遁甲書火候未到﹐還渡不出清氣。\n");
    if( me->query_score("magic") <= 10000 )
        return notify_fail("你的法術修為還淺﹐分不出餘力渡給旁人。\n");
    if( target->query_skill(skill, 1) <= 30 )
        return notify_fail("對方在這門功夫上根基太淺﹐承受不住你渡過去的清氣。\n");

    wis = me->query_attr("wis");
    spi = me->query_attr("spi");
    gain = ((wis > 0 ? random(wis) : 0) + 1) * ((spi > 0 ? random(spi) : 0) + 1)
        * random(me->query_skill("taoism of dunja", 1) / 20);
    me->delete_temp("dunja/experience");
    me->start_busy(1);

    if( gain <= 0 ) {
        message_vision("$N伸指點向$n眉心﹐那縷清氣卻在半途散了。\n", me, target, 1);
        return 1;
    }
    message_vision("$N伸指點在$n眉心﹐一縷清氣緩緩渡了過去。\n", me, target, 1);
    target->improve_skill_exact(skill, gain);
    me->gain_score("magic", -(gain / 10));
    tell_object(target, HIG "你只覺腦中豁然開朗﹐對" + to_chinese(skill) + "的領悟又深了一層。\n" NOR);
    return 1;
}

/* 對方身上的技能代碼：conjure 會把空格換成底線，兩種寫法都試。 */
string find_skill(object target, string spell)
{
    string alt;

    if( target->query_skill(spell, 1) || target->query_learn(spell) ) return spell;
    alt = replace_string(spell, "_", " ");
    if( target->query_skill(alt, 1) || target->query_learn(alt) ) return alt;
    return 0;
}

int conjure_magic(object me, string spell, object target)
{
    string skill;
    int fate, exp;

    if( !me->query_skill("taoism of dunja", 1) && !me->query_learn("taoism of dunja") )
        return notify_fail("你還沒學會遁甲書。\n");
    if( spell == "fate" ) return do_fate(me);
    if( spell == "experience" ) return do_experience(me);

    fate = me->query_temp("dunja/fate");
    exp = me->query_temp("dunja/experience");
    if( !fate && !exp )
        return notify_fail("你尚未推算天命﹐也未開啟心眼。\n");
    if( !objectp(target) )
        return notify_fail("指令格式﹕conjure <技能> on <對象>\n");
    if( target == me || !userp(target) || !living(target)
    ||  environment(target) != environment(me) )
        return notify_fail("遁甲之術只能施於旁人身上。\n");
    if( !(skill = find_skill(target, spell)) )
        return notify_fail(target->name() + "並未習得這門功夫﹐天機無從推算。\n");

    if( fate ) return read_fate(me, target, skill);
    return give_experience(me, target, skill);
}
