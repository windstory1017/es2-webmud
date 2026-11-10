// /custom/legacy_npc/yu_lan_jian_yi.c — 舊站 NPC：於蘭兼毅（Yu lan jian yi）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
舊站區域：天邪國；房間：—。
門派「天邪派」尚未列入門派武功對照表，先不設門派記號。
放在雪吟莊地牢深處 d/snowkeep/dungeon_e.c（2026-11-06）。
武功與戰鬥行為（使用者定案，2026-11-07）：會天邪神功、天邪神掌；進入戰鬥就擺出天邪掌
（perform unarmed:cele zarn，效果結束再擺）；戰鬥中會發出天邪虎嘯；氣或 HP 低於上限的一成時
強行催谷天邪神功，精氣神與 HP 全部補滿、傷害力 +50，每次重生最多兩次。
*/
#include <ansi.h>
inherit "/custom/legacy_npc/legacy_npc";

#define SURGE_MAX       2       // 強行催谷的次數（每次重生重新計算）
#define SURGE_RATIO     10      // 氣或 HP 低於上限的百分之幾時催谷
#define SURGE_DAMAGE    50      // 每次催谷提高的傷害力
#define ROAR_CHANCE     20      // 戰鬥中每次心跳發出天邪虎嘯的機率（%）

void create()
{
    seteuid(getuid());
    set_name("於蘭兼毅", ({ "yu lan jian yi", "yu", "yi" }));
    set("title", "天邪派掌門");
    set("nickname", "武林盟主");
    set_race("human");
    set_class("fighter");
    set_level(70);
    set("gender", "male");
    set("age", 55);
    set("long",
        "於蘭兼毅是當今的武林盟主﹐也是天邪國公獨生子﹐如果你以為他是\n"
        "靠著貴族的身分而獲得武林盟主的地位﹐你可是會大吃一驚的﹐天邪\n"
        "國一向是於蘭宗室掌管武術之秘的一支﹐宗室中的人物在武林之中向\n"
        "來依照武林規矩﹐於蘭兼毅在武林中成名已超過三十年﹐除了執掌天\n"
        "邪派門戶之外﹐也和武林中另外五位奇人合稱「六賢」。\n");
    set_skill("force", 200);
    set_skill("celestial force", 200);
    set_skill("unarmed", 200);
    set_skill("celestial palm", 200);
    map_skill("force", "celestial force");
    map_skill("unarmed", "celestial palm");
    set_power("S");
    setup();
    carry_object("/custom/armor/head/brass_hat")->wear();   // 黃呢鐵帽
    carry_object("/custom/armor/feet/spider_boots")->wear();   // 玄蛛皂白靴
    carry_object("/custom/armor/armor/dragon_armor")->wear();   // 怒龍錦冑
    carry_object("/custom/armor/hand/skin_gloves")->wear();   // 鹿皮手套
}

// ------------------------------------------------------------------
// 戰鬥行為：天邪掌、天邪虎嘯；氣或 HP 低於一成時強行催谷天邪神功，最多兩次。
// ------------------------------------------------------------------

private int low_stat(string what)
{
    int max = query_stat_maximum(what);
    return max > 0 && query_stat(what) * 100 < max * SURGE_RATIO;
}

private void surge()
{
    foreach( string what in ({ "gin", "kee", "sen", "HP" }) ) {
        if( query_stat_maximum(what) <= 0 ) continue;
        set_stat_effective(what, query_stat_maximum(what));
        set_stat_current(what, query_stat_maximum(what));
    }
    add_temp("apply/damage", SURGE_DAMAGE);
    add_temp("yu_lan/surge", 1);
    message_vision(HIR "\n$N雙目一睜﹐強行催谷天邪神功﹐週身骨節格格作響﹐一股霸道的真氣直衝百脈﹐"
        "傷勢竟在轉眼間盡數平復﹐出手更見凌厲！\n\n" NOR, this_object(), 0, 1);
}

static void heart_beat()
{
    ::heart_beat();
    if( !this_object() || !living(this_object()) || !is_fighting() ) return;

    if( query_temp("yu_lan/surge") < SURGE_MAX && (low_stat("kee") || low_stat("HP")) )
        surge();

    if( is_busy() ) return;
    if( !query_temp("celestial_palm/zarn") )
        perform_action("celestial palm", "cele zarn");
    else if( random(100) < ROAR_CHANCE )
        exert_function("roar");
}
