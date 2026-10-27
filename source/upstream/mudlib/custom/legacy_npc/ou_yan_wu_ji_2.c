// /custom/legacy_npc/ou_yan_wu_ji_2.c — 舊站 NPC：歐陽無極（Ou-yan wu ji）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
舊站區域：茅山、康平村；房間：小客店。
*/
#include <ansi.h>

inherit "/custom/legacy_npc/legacy_npc";

#define FACTION "taoist.wind"

void create()
{
    seteuid(getuid());
    set_name("歐陽無極", ({ "ou-yan wu ji", "ou-yan", "ji" }));
    set("title", "隱風觀觀主");
    set_race("human");
    set_class("taoist");
    set_level(40);
    set("gender", "male");
    set("age", 45);
    set("long",
        "隱風觀觀主本是四十多年前離開茅山﹐自創隱風觀的宇文太極。怎料數日前\n"
        "宇文太極竟忽然一命歸天﹐引得江湖中盛傳『茅山劍法』再顯與世。新任的\n"
        "隱風觀觀主﹐歐陽無極則是一口咬定宇文太極是忙於重建茅山﹐操勞過度而\n"
        "死。歐陽無極是宇文太極生前最鐘愛的弟子﹐而且在『茅山幻術』上的造詣\n"
        "頗深。他此時離開隱風觀據說是有事要和靈雲觀觀主和幻霧觀觀主商討。\n");
    set("custom_faction", FACTION);   // 隱風觀（docs/martial_arts/門派武功對照表.md）
    set_power("A");
    setup();
    carry_object("/custom/armor/cloth/green_robe")->wear();   // 青衣道袍
    carry_object("/custom/weapon/sword/peachwood_sword")->wield();   // 桃木劍
}

// ------------------------------------------------------------------
// 隱風觀師父（使用者定案，2026-10-21）：平民可拜師，但不收夜叉、刑天、厭火、黑齒；拜師後成為道士、隱風觀弟子。
// 傳授茅山共通法術（docs/martial_arts/茅山_法術.md）、茅山幻術、遁甲書
// （docs/martial_arts/隱風觀_茅山幻術與遁甲書.md）。
// ------------------------------------------------------------------

int accept_info(object ob, string type)
{
    return type == "skills";
}

int accept_apprentice(object me)
{
    if( me->query_class() != "commoner" ) {
        command("say 你已另有師承﹐貧道不便收你。");
        return 0;
    }
    // 隱風觀不收夜叉、刑天、厭火、黑齒（使用者指示，2026-10-21）。
    if( member_array(me->query_race(), ({ "yaksa", "headless", "yenhold", "blackteeth" })) >= 0 ) {
        command("say 你與本觀的道法無緣﹐還是另尋去處吧。");
        return 0;
    }
    return 1;
}

int init_apprentice(object me)
{
    if( ::init_apprentice(me) ) {
        me->set_class("taoist");
        me->set("title", "隱風觀弟子");
        me->set("custom_faction", FACTION);
        do_chat(({
            "歐陽無極撫了撫長鬚：好﹐從今天起你便是隱風觀的弟子了。\n",
            "歐陽無極說道：風無常勢﹐修道的人心思也要靈活些。\n",
        }));
    }
    return 1;
}

int acquire_skill(object me, string skill)
{
    if( me->query("custom_faction") != FACTION ) return 0;

    switch( skill ) {
    case "mao-shan magic":
    case "magic":
    case "taoism of nature":
    case "spells":
        break;
    case "taoism of dunja":
        // 茅山幻術 90 級、人物等級 30、靈性（不含裝備）30（使用者定案，2026-10-22）。
        if( !SKILL_D(skill)->valid_learn(me) ) return 0;
        break;
    default:
        return 0;
    }

    if( me->query_learn(skill) >= me->skill_threshold(skill, 1) )
        return notify_fail("歐陽無極說道：入門的要訣已經傳給你了﹐往後全看你自己。\n");

    me->improve_skill_exact(skill, me->skill_threshold(skill, 1) - me->query_learn(skill));
    tell_object(me, CYN + "歐陽無極將「" + to_chinese(skill) + "」的入門要訣傳授給你。\n" NOR);
    return 1;
}
