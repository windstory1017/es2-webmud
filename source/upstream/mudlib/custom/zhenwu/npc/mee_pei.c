#include <ansi.h>
#include <npc.h>

inherit F_SOLDIER;

void create()
{
    set_name("米沛", ({"mee-pei", "mee pei", "mee", "pei", "米沛"}));
    set_race("human");
    set_class("soldier"); set_level(40);
    set("gender", "male");
    set("nickname", "振武軍提督");



    set_skill("powerblow", 180);
    set_learn("powerblow", 1);

    // 狂擊由系統依軍人身分自動加入戰鬥動作（docs/CUSTOM_NPC_SKILL.md），這裡只設施展機率。
    set("chat_chance_combat", 50);

    set("long", "米沛是振武軍最高指揮官，身披戰甲，神情沉穩威嚴。他在軍中多年，以實戰與軍功一步步升任主將，看來約五十多歲。軍中傳聞具備相當實力的軍人可向他申請更進一步的職務，但相關條件目前仍待可靠資料補齊。\n");

    set_power("A");
    setup();

    carry_object("/custom/zhenwu/obj/steel_boots")->wear();
    carry_object("/custom/zhenwu/obj/fire_gauntlets")->wear();
    carry_object("/custom/zhenwu/obj/steel_armor")->wear();
    carry_object("/custom/zhenwu/obj/great_axe_of_mighty")->wield("twohanded axe");
    carry_object("/obj/books/book_of_assault");   // 【奇兵精意】（使用者指定，2026-10-16）
}

/* 強力格擋劇情：軍人（不是武者轉任）、人物 40 級以上、還沒學過強力格擋，走進大帳時觸發。
 * 戰場功勳 10 萬以上當場傳授；不足時這次登入不再觸發，直到戰功達標或重新登入。 */
#define HEAVY_PARRY_MERIT 100000

void init()
{
    object me = this_player();

    ::init();
    if( !userp(me) || me->query_class() != "soldier" || me->query_level() < 40 ) return;
    if( me->query_learn("heavy_parry") ) return;
    if( me->query("soldier/from_fighter") ) return;   // 武者轉任的軍人學不到軍人特殊技能
    if( me->query_temp("mee_pei/heavy_parry_refused")
    &&  me->query_score("military service") < HEAVY_PARRY_MERIT ) return;
    if( me->query_temp("mee_pei/heavy_parry_talking") ) return;

    me->set_temp("mee_pei/heavy_parry_talking", 1);
    call_out("heavy_parry_greet", 1, me);
}

void heavy_parry_greet(object me)
{
    if( !objectp(me) || environment(me) != environment() ) {
        if( objectp(me) ) me->delete_temp("mee_pei/heavy_parry_talking");
        return;
    }
    command("say 最近常聽兵士們談起你的事跡，今日一見，果然眼神犀利、身形沉穩。");
    call_out("heavy_parry_judge", 4, me);   // 停頓 2 個心跳
}

void heavy_parry_judge(object me)
{
    if( !objectp(me) ) return;
    me->delete_temp("mee_pei/heavy_parry_talking");
    if( environment(me) != environment() ) return;

    if( me->query_score("military service") < HEAVY_PARRY_MERIT ) {
        command("say 我會密切注意你。等你在戰場上創下一番功勳，再來見我吧。");
        me->set_temp("mee_pei/heavy_parry_refused", 1);
        return;
    }

    command("say 你戰功彪炳，我便傳你一套我在戰場上領悟的強力格擋之法，望你今後繼續保護百姓。");
    me->delete_temp("mee_pei/heavy_parry_refused");
    me->improve_skill_exact("heavy_parry", 1);
    tell_object(me, CYN + "米沛將強力格擋的要訣傳授給你。\n" NOR);
}

int accept_fight()
{
    return 0;
}
