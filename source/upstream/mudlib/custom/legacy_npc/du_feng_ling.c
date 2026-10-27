// /custom/legacy_npc/du_feng_ling.c — 舊站 NPC：杜風凌（Du feng ling）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。
位置：李家村小旅店（/d/lee/inn，使用者指示，2026-10-25）。
舊站區域：茅山、龍安鎮；房間：客棧。
*/
#include <ansi.h>

inherit "/custom/legacy_npc/legacy_npc";

#define FACTION "taoist.fog"

void create()
{
    seteuid(getuid());
    set_name("杜風凌", ({ "du feng ling", "du", "ling" }));
    set("title", "幻霧觀觀主");
    set_race("malik");
    set_class("taoist");
    set_level(40);
    set("gender", "male");
    set("age", 75);
    set("long",
        "杜風凌本是西方大漠的巫首族人﹐自幼拜林遠為師﹐修習茅山道法。也許是\n"
        "因為天賦異常﹐入門十數年就熟練的掌握了茅山道法的精髓並將其融會貫通,\n"
        "漸漸將一些不受重視的小把戲發展起來﹐當時盛傳杜風凌將會是下任茅山的\n"
        "掌門人。怎料當時的茅山掌門人﹐林遠﹐為了追查茅山劍法竟然屍橫荒野﹐\n"
        "一時間茅山派一片大亂。不少人因鄙視杜風凌所擅長的『茅山奇術』竟然離\n"
        "開茅山自成流派﹐而杜風凌也因其家族牽扯到黃泉村一事﹐而不得不暫回西\n"
        "方大漠。當他返回天朝以後﹐無法說服他的師兄弟重歸茅山﹐便憤然再返大\n"
        "漠﹐借助他家族在大漠的勢力自創『幻霧觀』。此次不知是為何事﹐竟然又\n"
        "來到天朝境內。\n");
    set("custom_faction", FACTION);   // 幻霧觀（docs/martial_arts/門派武功對照表.md）
    set_power("A");
    setup();
    carry_object("/custom/armor/cloth/green_robe")->wear();   // 青衣道袍
    carry_object("/custom/weapon/sword/peachwood_sword")->wield();   // 桃木劍
    carry_object("/custom/armor/finger/han_jade_ring")->wear();   // 漢玉戒指
}

// ------------------------------------------------------------------
// 幻霧觀師父（使用者定案，2026-10-21）：任何種族的平民都可拜師，拜師後成為道士、幻霧觀弟子。
// 傳授茅山共通法術（docs/martial_arts/茅山_法術.md），以及幻霧觀的茅山奇術、奇門術
// （docs/martial_arts/幻霧觀_茅山奇術與奇門術.md）。
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
    // 幻霧觀只收阿修羅、巫首、雨師妾、無腸、夜叉（使用者指示，2026-10-25）。
    if( member_array(me->query_race(), ({ "ashura", "malik", "rainner", "woochan", "yaksa" })) < 0 ) {
        command("say 你與本觀的道法無緣﹐還是另尋去處吧。");
        return 0;
    }
    return 1;
}

int init_apprentice(object me)
{
    if( ::init_apprentice(me) ) {
        me->set_class("taoist");
        me->set("title", "幻霧觀弟子");
        me->set("custom_faction", FACTION);
        do_chat(({
            "杜風凌淡淡地點了點頭：好﹐從今以後你便是幻霧觀的弟子。\n",
            "杜風凌說道：旁人瞧不起的小把戲﹐用得好了一樣能成大事。\n",
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
    case "taoism of darkness":
    case "spells":
        break;
    case "mao-shan mysticism":
        // 茅山奇術 100 級（使用者定案，2026-10-24）。
        if( !SKILL_D(skill)->valid_learn(me) ) return 0;
        break;
    default:
        return 0;
    }

    if( me->query_learn(skill) >= me->skill_threshold(skill, 1) )
        return notify_fail("杜風凌說道：入門的要訣已經傳給你了﹐往後全看你自己。\n");

    me->improve_skill_exact(skill, me->skill_threshold(skill, 1) - me->query_learn(skill));
    tell_object(me, CYN + "杜風凌將「" + to_chinese(skill) + "」的入門要訣傳授給你。\n" NOR);
    return 1;
}
