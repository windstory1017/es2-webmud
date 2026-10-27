#include <ansi.h>
// /d/oldpine/npc/kao_shen.c — 老松林 NPC：虎刀門長老 高慎（放在老松林北緣的空地）。
/*
CUSTOM LEGACY-RESEARCH NPC PLACEMENT.
Identity/description grounded in user-authorized legacy ES2 research.
Apprenticeship and teacher skill table grounded in user-provided legacy teacher output.
Special-skill combat formulas remain intentionally unimplemented until source evidence is found.
*/
#include <npc.h>

inherit F_VILLAGER;

void create()
{
    set_name("高慎", ({ "kao shen", "kao", "shen"}));
    set("title", "虎刀門長老");
    set_race("human");
    set("gender", "male");
    set_class("fighter"); set_level(30);

    /* User-provided legacy teacher table: teaching ceilings. */
    set_skill("tiger-steps", 120);
    set_skill("ksanmando", 140);
    set_skill("blade", 120);
    set_skill("twohanded blade", 120);
    set_skill("secondhand blade", 120);
    set_skill("force", 120);
    set_skill("tiger force", 120);
    set_skill("tiger-blade", 140);
    set("long",
        "高慎是季縣虎刀門的耆宿，雖已年逾七旬，身形仍顯得十分硬朗。\n"
        "他眉目間帶著一股火爆剛烈之氣，站在老松林中也像一柄尚未入鞘的刀。\n"
        "據說他這次離開虎刀門，正是為了在外收幾名合意弟子，好讓門中後輩\n"
        "在一年一度的試會上爭一口氣。他看來膽識過人，一副天不怕地不怕的模樣。\n");
    set("chat_chance", 4);
    set("chat_msg", ({
        "高慎負手望著林間小路，冷哼一聲，神情頗為不耐。\n",
        "高慎伸手按了按腰間刀鞘，目光銳利地打量四周。\n",
    }));
    set("custom_faction", "fighter.tiger");   // 門派記號（docs/CUSTOM_NPC_SKILL.md）
    set_power("B");
    setup();
}

int accept_info(object ob, string type)
{
    return type == "skills";
}

/* 拜師條件：平民（尚未加入任何職業），且刀法至少 1 級。拜師後成為武者（虎刀門弟子）。 */
int accept_apprentice(object me)
{
    if( me->query_class() != "commoner" ) {
        command("say 你已經另有所屬, 不要想套近乎。");
        return 0;
    }

    if( me->query_skill("blade", 1) < 1 ) {
        command("say 你連刀都不會用, 回去多練幾招再來。");
        return 0;
    }

    return 1;
}

int init_apprentice(object me)
{
    if( ::init_apprentice(me) ) {
        me->set_class("fighter");
        me->set("title", "虎刀門弟子");
        me->set("custom_faction", "fighter.tiger");
        do_chat(({
            "高慎上下打量了你一番，忽然哈哈大笑道：好！既然敢拜，老夫便敢教！\n",
            "高慎沉聲說道：從今日起，你便是我虎刀門弟子。\n",
        }));
    }
    return 1;
}

/*
 * Acquisition gates from the user-provided legacy teacher output.
 * This only grants the skill seed. Combat formulas/actions for the three
 * tiger special skills are NOT invented here.
 */
int acquire_skill(object me, string skill)
{
    int cap;

    if( !me->is_apprentice_of(this_object()) ) return 0;

    switch(skill) {
    case "unarmed": cap = 1; break;
    case "parry": cap = 1; break;
    case "dodge": cap = 1; break;
    case "tiger-steps": cap = 1; break;
    case "ksanmando": cap = 1; break;
    case "blade": cap = 1; break;
    case "twohanded blade": cap = 1; break;
    case "secondhand blade": cap = 1; break;
    case "force":
        if( me->query_level() < 15 )
            return notify_fail("高慎搖頭道：你根基未穩，還是多練練吧。\n");
        cap = 1;
        break;
    case "tiger force":
        if( me->query_level() < 15 )
            return notify_fail("高慎搖頭道：你根基未穩，還是多練練吧。\n");
        cap = 1;
        break;
    case "tiger-blade":
        if( me->query_skill("tiger force", 1) < 30 )
            return notify_fail("高慎喝道：你的瘋虎功火候未到，也敢妄學瘋虎刀法？\n");
        cap = 1;
        break;
    default:
        return 0;
    }

    /* 只傳授入門：累積點數還不到 1 級的門檻才教，一次補足到門檻。
     * 等級要玩家下 gain 才會提升；瘋虎功要自己修習到 15 級的門檻（10000 點）才練成。 */
    if( me->query_learn(skill) >= me->skill_threshold(skill, cap) )
        return notify_fail("高慎說道：滾你奶奶的! 自己好好修練去!\n");

    me->improve_skill_exact(skill,
        me->skill_threshold(skill, cap) - me->query_learn(skill));
    if( skill == "tiger force" )
        tell_object(me, CYN + "高慎將瘋虎功的入門心訣傳授給你，接下來就得靠你自己運功修習了。\n" NOR);
    else
        tell_object(me, CYN + "高慎將「" + to_chinese(skill) + "」的入門要訣傳授給你。\n" NOR);
    return 1;
}

int accept_fight()
{
    command("say 老夫今日是來收徒，不是陪你胡鬧的！");
    return 0;
}
