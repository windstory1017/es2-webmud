// /custom/legacy_npc/yu_tieshan.c — 舊站 NPC：魚鐵山（Yu tieshan）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。放在雪吟莊大廳 d/snowkeep/inner_hall.c（2026-11-06）。
舊站區域：雪吟莊；房間：雪吟莊大廳。
雪吟莊師父：收徒、傳授雪吟莊武功（使用者定案，2026-11-05；docs/martial_arts/雪吟莊_武功.md 第六節）。
戰鬥行為（使用者定案，2026-11-06）：內功用天邪神功，戰鬥中會發出天邪虎嘯；
氣或 HP 低於上限的一成時吞丹補滿（每次重生最多四顆）。
*/
#include <ansi.h>
inherit "/custom/legacy_npc/legacy_npc";

#define FACTION "fighter.snow"

#define PILL_MAX        4       // 丹藥的數量（每次重生重新計算）
#define PILL_RATIO      10      // 氣或 HP 低於上限的百分之幾時吞丹
#define ROAR_CHANCE     20      // 戰鬥中每次心跳發出天邪虎嘯的機率（%）

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
    map_skill("force", "celestial force");   // 內功用天邪神功（等級由 NPC 自動技能給）
    set_power("S");
    setup();
    carry_object("/custom/armor/head/brass_hat")->wear();   // 黃呢鐵帽
    carry_object("/custom/armor/waist/water_dragon_girdle")->wear();   // 蛟龍帶
    carry_object("/custom/armor/feet/spider_boots")->wear();   // 玄蛛皂白靴
    carry_object("/custom/armor/cloth/golden_robe")->wear();   // 金絲長袍
    carry_object("/custom/armor/finger/ring_of_mighty_dragon")->wear();   // 巨龍指環
    carry_object("/custom/weapon/blade/golden_blade")->wield();   // 金絲八卦刀
    carry_object("/d/snowkeep/obj/brass_key");   // 黃銅鑰匙：開地牢深處的牢門（docs/AREA_SNOWKEEP.md）
}

// ------------------------------------------------------------------
// 戰鬥行為：天邪虎嘯；氣或 HP 低於一成時吞丹補滿，最多四顆。
// ------------------------------------------------------------------

private int low_stat(string what)
{
    int max = query_stat_maximum(what);
    return max > 0 && query_stat(what) * 100 < max * PILL_RATIO;
}

private void take_pill()
{
    foreach( string what in ({ "kee", "HP" }) ) {
        if( query_stat_maximum(what) <= 0 ) continue;
        set_stat_effective(what, query_stat_maximum(what));
        set_stat_current(what, query_stat_maximum(what));
    }
    add_temp("yu_tieshan/pill", 1);
    // 整句一行、前後各空一行（使用者指示，2026-11-06）。
    message_vision(HIG "\n$N從懷中摸出把丹藥一口吞了下去﹐傷勢在轉眼間盡數平復！\n\n" NOR,
        this_object(), 0, 1);
}

static void heart_beat()
{
    ::heart_beat();
    if( !this_object() || !living(this_object()) || !is_fighting() ) return;

    if( query_temp("yu_tieshan/pill") < PILL_MAX && (low_stat("kee") || low_stat("HP")) )
        take_pill();

    if( !is_busy() && random(100) < ROAR_CHANCE )
        exert_function("roar");
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
