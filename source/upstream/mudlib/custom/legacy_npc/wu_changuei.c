// /custom/legacy_npc/wu_changuei.c — 舊站 NPC：吳長貴（Wu changuei）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。尚未放入地圖。
瑯夷派師父（使用者定案，2026-11-04）：任何未加入門派的盜賊都可拜師，不限種族、性別；
拜師後成為「瑯夷派弟子」。瑯夷派內功見 docs/martial_arts/瑯夷派_瑯夷派內功.md。
舊站區域：未能判定；房間：—（房間尚未製作）。
尚未製作的裝備（先空著）：點鋼匕(Steel dagger)、襯甲(Padded armor)。
*/
#include <ansi.h>

inherit "/custom/legacy_npc/legacy_npc";

#define FACTION "thief.longye"

void create()
{
    seteuid(getuid());
    set_name("吳長貴", ({ "wu changuei", "wu", "changuei" }));
    set("nickname", "無常鬼");
    set_race("human");
    set_class("thief");
    set_level(15);
    set("gender", "male");
    set("age", 35);
    set("long",
        "一個面皮白淨的高瘦漢子﹐左頰上一道長長的刀疤﹐你突然想起這個\n"
        "漢子就是這一帶著名的飛賊「無常鬼」吳長貴。\n");
    set("custom_faction", "thief.longye");   // 瑯夷派（docs/martial_arts/門派武功對照表.md）
    set_power("B");
    setup();
    carry_object("/obj/area/obj/dagger")->wield();   // 匕首
    carry_object("/custom/armor/head/ghost_mask")->wear();   // 鬼面
    carry_object("/custom/armor/cloth/black_suit")->wear();   // 黑獄服
}

// ------------------------------------------------------------------
// 拜師：任何未加入門派的盜賊都收，不限種族、性別；拜師後成為瑯夷派弟子。
// ------------------------------------------------------------------

int accept_info(object ob, string type)
{
    return type == "skills";
}

int accept_apprentice(object me)
{
    string faction = me->query("custom_faction"), title = me->query("title");

    if( me->query_class() != "thief" ) {
        command("say 咱們瑯夷派只收道上混飯吃的兄弟﹐你還是請回吧。");
        return 0;
    }
    if( (stringp(faction) && faction != "" && faction != FACTION)
    ||  (stringp(title) && strsrch(title, "隱教") != -1) ) {
        command("say 你已經拜了別家的山頭﹐我吳長貴可不搶人。");
        return 0;
    }
    return 1;
}

int init_apprentice(object me)
{
    if( ::init_apprentice(me) ) {
        me->set("title", "瑯夷派弟子");
        me->set("custom_faction", FACTION);
        do_chat(({
            "吳長貴摸了摸臉上的刀疤﹐咧嘴一笑：好﹐從今往後你就是瑯夷派的弟子了。\n",
            "吳長貴壓低聲音說道：記住﹐幹咱們這行的﹐腳底要輕﹐手要快﹐心要更狠。\n",
        }));
    }
    return 1;
}

// ------------------------------------------------------------------
// 傳授：瑯夷派弟子才教；每門只補足到剛好可以升到 1 級的經驗。
// ------------------------------------------------------------------

int acquire_skill(object me, string skill)
{
    if( me->query("custom_faction") != FACTION ) return 0;

    switch( skill ) {
    case "longyeforce":
    case "force":
        if( me->query_level() < 5 )
            return notify_fail("吳長貴搖頭道：你的根基尚淺﹐還摸不透本門內功的門道。\n");
        break;
    default:
        return 0;
    }

    if( me->query_learn(skill) >= me->skill_threshold(skill, 1) )
        return notify_fail("吳長貴說道：入門的訣竅都教給你了﹐往後自己去琢磨。\n");

    me->improve_skill_exact(skill, me->skill_threshold(skill, 1) - me->query_learn(skill));
    tell_object(me, CYN + "吳長貴將「" + to_chinese(skill) + "」的入門訣竅傳授給你。\n" NOR);
    return 1;
}
