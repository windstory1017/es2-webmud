// /custom/legacy_npc/taoist_leader.c — 舊站 NPC：白齊（Taoist leader）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。
舊站區域：太乙觀；房間：賓客宿房。
2026-11-09 起放在新做的太乙觀正殿（`d/taiyi/main_hall`，設計見 docs/AREA_TAIYI.md），是素衣派掌門：
不限種族與性別的平民都可拜師（已有師承不收），拜師後成為道士、「素衣派弟子」；
傳授天師道法【冰咒】、咒術、劍術、閃躲、招架，人物等級 15 起傳授冰蠶奇功（使用者定案）。
*/
#include <ansi.h>

inherit "/custom/legacy_npc/legacy_npc";

#define FACTION "taoist.freeze"

void create()
{
    seteuid(getuid());
    set_name("白齊", ({ "taoist leader", "taoist", "leader" }));
    set("title", "太乙觀觀主");
    set_race("human");
    set_class("taoist");
    set_level(50);
    set("gender", "male");
    set("age", 75);
    set("long",
        "在你眼前的是一位仙風道骨的道士，身著一件淡青色道袍，渾身\n"
        "散發出一股悠然的風範與氣度，他就是太乙觀的觀主--白齊。太\n"
        "乙觀內的上清道長，玉清道長跟玄清道長都是他的得意門徒，他\n"
        "對待觀內的小道童，道士們也都很親切，可以說是太乙觀中最具\n"
        "有影響力的人物。\n");
    set("custom_faction", "taoist.freeze");   // 素衣派（docs/martial_arts/門派武功對照表.md）
    set("chat_chance_combat", 50);
    set("chat_msg_combat", ({
        (: command, "cast 1" :),
        (: command, "cast 2" :),
        (: command, "cast 3" :),
        (: command, "cast 4" :)
    }));
    set_power("A");
    setup();
    carry_object("/custom/weapon/sword/shome_sword")->wield();   // 少眉劍
    carry_object("/custom/armor/feet/black_cloth_boots")->wear();   // 黑布鞋
    carry_object("/custom/armor/cloth/green_robe")->wear();   // 青衣道袍
}

// ------------------------------------------------------------------
// 拜師：不限種族與性別的平民都收；拜師後成為道士、素衣派弟子。
// ------------------------------------------------------------------

int accept_info(object ob, string type)
{
    return type == "skills";
}

int accept_apprentice(object me)
{
    if( me->query_class() != "commoner" ) {
        command("say 閣下已有師承﹐貧道不便收你。");
        return 0;
    }
    return 1;
}

int init_apprentice(object me)
{
    if( ::init_apprentice(me) ) {
        me->set_class("taoist");
        me->set("title", "素衣派弟子");
        me->set("custom_faction", FACTION);
        do_chat(({
            "白齊撫著長鬚微微一笑﹐說道：好﹐從今天起你便是素衣派的弟子了。\n",
            "白齊說道：冰咒以靜制動﹐心不亂則寒氣自生。好生修持﹐莫負了素衣一脈的清名。\n",
        }));
    }
    return 1;
}

// ------------------------------------------------------------------
// 傳授：素衣派弟子才教；每門只補足到剛好可以升到 1 級的經驗。
// ------------------------------------------------------------------

int acquire_skill(object me, string skill)
{
    if( me->query("custom_faction") != FACTION ) return 0;

    switch( skill ) {
    case "taoism-freeze":
    case "spells":
    case "sword":
    case "dodge":
    case "parry":
        break;
    case "polar-silkworm force":
    case "force":
        if( me->query_level() < 15 )
            return notify_fail("白齊搖頭道：你根基尚淺﹐還承受不住冰蠶奇功的寒勁。\n");
        break;
    default:
        return 0;
    }

    if( me->query_learn(skill) >= me->skill_threshold(skill, 1) )
        return notify_fail("白齊說道：入門的要訣已經傳給你了﹐往後全看你自己。\n");

    me->improve_skill_exact(skill, me->skill_threshold(skill, 1) - me->query_learn(skill));
    tell_object(me, CYN + "白齊將「" + to_chinese(skill) + "」的入門要訣傳授給你。\n" NOR);
    return 1;
}
