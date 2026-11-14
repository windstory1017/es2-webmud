// /custom/legacy_npc/ro_yi.c — 舊站 NPC：駱易（Ro yi）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。放在步玄派沐華居（d/swaylan/buxuan/residence.c）。
步玄派師父（使用者定案，2026-11-01）：種族、性別不限；讀書識字 30 以上的平民，或已是書生，
可拜師，拜師後成為書生、「步玄派弟子」（門派代碼 scholar.myst）。
武功見 docs/martial_arts/步玄派_門派設計.md。
兵器（使用者定案，2026-11-01）：右手風泉之劍（唯一；被取走後重生改拿逐風刃）、左手落雲刃。
*/
#include <ansi.h>
inherit "/custom/legacy_npc/legacy_npc";

#define FACTION "scholar.myst"

void create()
{
    seteuid(getuid());
    set_name("駱易", ({ "ro yi", "ro", "yi" }));
    set("title", "步玄派掌門");
    set_race("human");
    set_class("scholar");
    set_level(65);
    set("gender", "male");
    set("age", 55);
    set("long",
        "駱易本人性格恬淡，雖然身為步玄派掌門人，卻極為謙退自抑，外人只知道駱易\n"
        "少年時曾孤身一人犯險闖入西方大漠的飛狼城，救回當今的皇帝及香雪王，成為\n"
        "轟動一時的人物。駱易辭卻了朝廷及香雪王的豐厚賞賜及高官厚祿，隱居在喬陰\n"
        "的祖宅，也成為江湖傳頌的美談。\n");
    set("custom_faction", FACTION);   // 步玄派；武功等級依門派自動配置（daemon/misc/npc_skill.c）
    // 精、氣、神固定為 1800／2200／1000（使用者指示，2026-11-12），不隨強度自動計算。
    set_stat_maximum("gin", 1800);
    set_stat_maximum("kee", 2200);
    set_stat_maximum("sen", 1000);
    set_power("S");
    setup();
    foreach( string st, int v in ([ "gin": 1800, "kee": 2200, "sen": 1000 ]) ) {
        set_stat_effective(st, v);
        set_stat_current(st, v);
    }
    carry_object("/custom/armor/armor/black_harness")->wear();   // 玄晦戰甲
    carry_object("/custom/armor/finger/han_jade_ring")->wear();   // 漢玉戒指
    carry_object("/custom/armor/feet/snakeskin_boots")->wear();   // 青蟒靴
    carry_object("/custom/armor/waist/white_jade_girth")->wear();   // 流雲清玉帶
    carry_object("/custom/armor/cloth/golden_robe")->wear();   // 金絲長袍
    carry_object("/custom/armor/head/headgear")->wear();   // 紫紗逍遙巾
    carry_object("/custom/weapon/sword/sword_of_wind_spring")->wield("sword");   // 風泉之劍（唯一；已被取走時換成逐風刃）
    carry_object("/custom/weapon/sword/cloudslay_sword")->wield("secondhand sword");   // 落雲刃
    carry_object("/obj/books/water_dragon_canon");   // 【水龍經】（舊站資料，使用者指示，2026-11-12）
}

// ------------------------------------------------------------------
// 拜師：種族、性別不限；讀書識字 30 以上的平民，或已是書生（使用者定案，2026-11-01）。
// 已拜入其他書生門派（例如玄天派，custom_faction 為 scholar.sky）的不收（使用者定案，2026-11-01）。
// 拜師後成為書生、步玄派弟子。
// ------------------------------------------------------------------

int accept_info(object ob, string type)
{
    return type == "skills";
}

int accept_apprentice(object me)
{
    string cls = me->query_class(), faction = me->query("custom_faction"), title = me->query("title");

    if( cls == "scholar" ) {
        if( (stringp(faction) && faction != "" && faction != FACTION)
        ||  (stringp(title) && strsrch(title, "玄天") != -1) ) {
            command("say 閣下已是別派門下，駱某不敢奪人所好。");
            return 0;
        }
        return 1;
    }
    if( cls != "commoner" ) {
        command("say 閣下已有師承，駱某不敢奪人所好。");
        return 0;
    }
    if( me->query_skill("literate", 1) < 30 ) {
        command("say 本門武功出自書卷，閣下的學問根基尚淺，還請多讀些書再來。");
        return 0;
    }
    return 1;
}

int init_apprentice(object me)
{
    if( ::init_apprentice(me) ) {
        me->set_class("scholar");
        me->set("title", "步玄派弟子");
        me->set("custom_faction", FACTION);
        do_chat(({
            "駱易溫言道：好，從今日起，你便是我步玄派弟子。\n",
            "駱易說道：本門武功以身法為先，進退之間都要從容，切莫急躁。\n",
        }));
    }
    return 1;
}

// ------------------------------------------------------------------
// 傳授：步玄派弟子才教；每門只補足到剛好可以升到 1 級的經驗。
// ------------------------------------------------------------------

int acquire_skill(object me, string skill)
{
    if( me->query("custom_faction") != FACTION ) return 0;

    switch( skill ) {
    case "close myst-sword":
    case "myst-steps":
    case "sword":
    case "secondhand sword":
    case "parry":
    case "dodge":
        break;
    case "myst-force":
    case "force":
        if( me->query_level() < 5 )
            return notify_fail("駱易搖頭道：你的根基尚淺，還領會不了本門心法。\n");
        break;
    default:
        return 0;
    }

    if( me->query_learn(skill) >= me->skill_threshold(skill, 1) )
        return notify_fail("駱易說道：入門的要訣已經傳給你了，往後全看你自己。\n");

    me->improve_skill_exact(skill, me->skill_threshold(skill, 1) - me->query_learn(skill));
    tell_object(me, CYN + "駱易將「" + to_chinese(skill) + "」的入門要訣傳授給你。\n" NOR);
    return 1;
}
