// /custom/legacy_npc/yu_tieshan.c — 舊站 NPC：魚鐵山（Yu tieshan）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
舊站區域：雪吟莊；房間：雪吟莊大廳。
雪吟莊師父：收徒、傳授雪吟莊武功（使用者定案，2026-11-05；docs/martial_arts/雪吟莊_武功.md 第六節）。
*/
#include <ansi.h>
inherit "/custom/legacy_npc/legacy_npc";

#define FACTION "fighter.snow"

void create()
{
    seteuid(getuid());
    set_name("魚鐵山", ({ "yu tieshan", "yu", "tieshan" }));
    set("title", "雪吟莊莊主");
    set_race("human");
    set_class("fighter");
    set_level(65);
    set("gender", "male");
    set("age", 65);
    set("long",
        "魚鐵山是雪吟莊的莊主，也是當今武林號稱「六賢」之一的武林高手，魚鐵山\n"
        "成名已近四十年，和當今武林盟主於蘭兼毅是總角之交，曾一起在天邪派前掌\n"
        "門於蘭焱門下學藝，不過由於於蘭兼毅是天邪國公世子，因此於蘭焱收於蘭兼\n"
        "毅為師弟，而魚鐵山反而要稱這位小他三歲的老友師叔了。\n");
    set("custom_faction", FACTION);   // 雪吟莊（docs/martial_arts/門派武功對照表.md）
    set_power("S");
    setup();
    carry_object("/custom/armor/head/brass_hat")->wear();   // 黃呢鐵帽
    carry_object("/custom/armor/waist/water_dragon_girdle")->wear();   // 蛟龍帶
    carry_object("/custom/armor/feet/spider_boots")->wear();   // 玄蛛皂白靴
    carry_object("/custom/armor/cloth/golden_robe")->wear();   // 金絲長袍
    carry_object("/custom/armor/finger/ring_of_mighty_dragon")->wear();   // 巨龍指環
    carry_object("/custom/weapon/blade/golden_blade")->wield();   // 金絲八卦刀
}

// ------------------------------------------------------------------
// 拜師
// ------------------------------------------------------------------

int accept_info(object ob, string type)
{
    return type == "skills";
}

int accept_apprentice(object me)
{
    if( me->query_class() != "commoner" ) {
        command("say 閣下已有師承，老夫又何必奪人所好？");
        return 0;
    }
    return 1;
}

int init_apprentice(object me)
{
    if( ::init_apprentice(me) ) {
        me->set_class("fighter");
        me->set("title", "雪吟莊弟子");
        me->set("custom_faction", FACTION);
        do_chat(({
            "魚鐵山捋鬚而笑：好，從今日起，你便是我雪吟莊的弟子了。\n",
            "魚鐵山正色說道：雪吟莊在江湖上名頭不小，你在外行走，莫要墮了本莊的威風。\n",
        }));
    }
    return 1;
}

// ------------------------------------------------------------------
// 傳授：雪吟莊弟子才教；每門只補足到剛好可以學成的經驗。
// 武功本身的學習條件由武功檔的 valid_learn() 判定。
// ------------------------------------------------------------------

int acquire_skill(object me, string skill)
{
    object daemon;

    if( me->query("custom_faction") != FACTION ) return 0;

    switch( skill ) {
    case "blade":
    case "sword":
    case "parry":
    case "dodge":
    case "unarmed":
        break;
    case "force":
        if( me->query_level() < 5 )
            return notify_fail("魚鐵山搖頭道：你的根基尚淺，內功的事過些日子再說。\n");
        break;
    case "tidy force":
    case "deep blade":
    case "six-chaos-sword":
    case "celestial force":
    case "celestial palm":
        daemon = SKILL_D(skill);
        if( objectp(daemon) && !daemon->valid_learn(me) ) return 0;
        break;
    default:
        return 0;
    }

    if( me->query_learn(skill) >= me->skill_threshold(skill, 1) )
        return notify_fail("魚鐵山淡淡說道：入門的要訣已經傳給你了，往後全看你自己。\n");

    me->improve_skill_exact(skill, me->skill_threshold(skill, 1) - me->query_learn(skill));
    tell_object(me, CYN + "魚鐵山將「" + to_chinese(skill) + "」的入門要訣傳授給你。\n" NOR);
    return 1;
}
