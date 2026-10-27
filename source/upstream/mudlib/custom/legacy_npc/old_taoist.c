// /custom/legacy_npc/old_taoist.c — 舊站 NPC：安懶道人（Old taoist）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
玄衣派師父（使用者定案，2026-10-21）：任何種族的平民都可拜師，但靈性要 18 以上；
拜師後成為「玄衣派弟子」。明道心法見 docs/martial_arts/玄衣派_明道心法.md。
舊站區域：天邪國；房間：草廬。
*/
#include <ansi.h>

inherit "/custom/legacy_npc/legacy_npc";

#define FACTION "taoist.storm"

void create()
{
    seteuid(getuid());
    set_name("安懶道人", ({ "old taoist", "old", "taoist" }));
    set_race("human");
    set_class("taoist");
    set_level(60);
    set("gender", "male");
    set("age", 85);
    set("long",
        "安懶道人是個鬍鬚花白的老道士，他曾經在天邪國擔任御史大夫達三十多\n"
        "年，後來辭官入山學道。\n");
    set("custom_faction", "taoist.storm");   // 玄衣派（docs/martial_arts/門派武功對照表.md）
    set("chat_chance_combat", 50);
    set("chat_msg_combat", ({
        (: command, "cast 1" :),
        (: command, "cast 2" :),
        (: command, "cast 3" :),
        (: command, "cast 4" :)
    }));
    set_power("A");
    setup();
    carry_object("/custom/armor/finger/sapphire_ring")->wear();   // 藍水晶戒指
    carry_object("/custom/armor/cloth/tao_robe")->wear();   // 杏黃道袍
    carry_object("/custom/armor/armor/magic_armor")->wear();   // 法胄
    carry_object("/custom/weapon/sword/sword_of_raven")->wield();   // 黃雀寶劍
    carry_object("/custom/weapon/sword/peachwood_sword");   // 桃木劍
}

// ------------------------------------------------------------------
// 拜師：任何種族的平民都收，但靈性（原始值）要 18 以上；拜師後成為道士、玄衣派弟子。
// ------------------------------------------------------------------

int accept_info(object ob, string type)
{
    return type == "skills";
}

int accept_apprentice(object me)
{
    if( me->query_class() != "commoner" ) {
        command("say 閣下已有師承﹐老道不便收你。");
        return 0;
    }
    if( me->query_attr("spi", 1) < 18 ) {
        command("say 你資質少了幾分靈氣﹐只怕悟不透本門的道法。");
        return 0;
    }
    return 1;
}

int init_apprentice(object me)
{
    if( ::init_apprentice(me) ) {
        me->set_class("taoist");
        me->set("title", "玄衣派弟子");
        me->set("custom_faction", FACTION);
        do_chat(({
            "安懶道人睜開半瞇的眼睛﹐慢吞吞地說道：好吧﹐從今天起你就是玄衣派的弟子了。\n",
            "安懶道人說道：修道急不得﹐凡事慢慢來﹐別學老道當年在朝為官那般汲汲營營。\n",
        }));
    }
    return 1;
}

// ------------------------------------------------------------------
// 傳授：玄衣派弟子才教；每門只補足到剛好可以升到 1 級的經驗。
// ------------------------------------------------------------------

int acquire_skill(object me, string skill)
{
    if( me->query("custom_faction") != FACTION ) return 0;

    switch( skill ) {
    case "taoism-storm":
    case "spells":
    case "sword":
    case "dodge":
    case "parry":
        break;
    case "dao force":
    case "force":
        if( me->query_level() < 20 )
            return notify_fail("安懶道人搖頭道：你根基尚淺﹐還領會不了明道心法。\n");
        break;
    default:
        return 0;
    }

    if( me->query_learn(skill) >= me->skill_threshold(skill, 1) )
        return notify_fail("安懶道人說道：入門的要訣已經傳給你了﹐往後全看你自己。\n");

    me->improve_skill_exact(skill, me->skill_threshold(skill, 1) - me->query_learn(skill));
    tell_object(me, CYN + "安懶道人將「" + to_chinese(skill) + "」的入門要訣傳授給你。\n" NOR);
    return 1;
}
