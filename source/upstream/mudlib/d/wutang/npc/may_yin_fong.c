#include <ansi.h>
// /d/wutang/npc/may_yin_fong.c — 五堂鎮 NPC：冷梅莊莊主 梅影風。
/*
CUSTOM NPC (NEW). 設定來源：使用者提供（外觀敘述、裝備、技能、拜師與傳授條件）。
在交叉路口出生，只在五堂鎮內走動。武功見 docs/martial_arts/冷梅莊_劍士武功.md。
*/
#include <npc.h>
#include <command.h>

inherit F_FIGHTER;

#define HOME_ROOM       "/d/wutang/crossroad"
#define AREA_PREFIX     "/d/wutang/"
#define FACTION         "fighter.lunmay"
#define WHITE_GIRTH     "/custom/armor/waist/white_girth"
#define SILVER_GIRTH    "/custom/armor/waist/silver_girth"

// 每 2 tick（4 秒）擲一次，10% 機率移動。
#define WANDER_INTERVAL 4
#define WANDER_CHANCE   10

void create()
{
    object girth;

    seteuid(getuid());
    set_name("梅影風", ({ "may yin fong", "may", "fong" }));
    set("title", "冷梅莊莊主");
    set_race("human");
    set_class("fighter");
    set_level(60);
    set("gender", "male");
    set("age", 55);
    set("long",
        "你面前的是一位兩鬢霜白的中年人，眉宇之間流露著一股冷傲的神情。冷梅莊莊\n"
        "主梅影風看來便是他了。從他的臉上你可以看出他似乎是很煩惱的樣子。\n");

    set_skill("lunmay", 180);
    set_skill("sword", 180);
    set_skill("force", 190);
    set_skill("hainmay force", 200);
    set_skill("secondhand sword", 180);
    set_skill("secondhand dagger", 180);
    set_skill("advance_lunmay", 180);
    set_skill("parry", 160);
    set_skill("dodge", 150);
    set_skill("mayin", 160);
    set_skill("unarmed", 140);
    set_skill("seven", 160);
    map_skill("sword", "lunmay");
    map_skill("parry", "lunmay");
    map_skill("secondhand dagger", "advance_lunmay");
    map_skill("secondhand sword", "advance_lunmay");
    map_skill("force", "hainmay force");
    map_skill("dodge", "mayin");
    map_skill("unarmed", "seven");

    set("custom_faction", FACTION);   // 門派記號（docs/CUSTOM_NPC_SKILL.md）
    set_power("S");
    setup();

    carry_object("/custom/armor/armor/silver_platemail_of_frost")->wear();
    carry_object("/custom/armor/feet/boots_of_flying_cloud")->wear();
    carry_object("/custom/armor/cloth/white_robe")->wear();
    carry_object("/custom/armor/finger/white_ring")->wear();
    // 白玉腰帶世上只有一條；已經在別人手上時改穿銀彎束腰。
    if( clonep() ) {
        girth = carry_object(WHITE_GIRTH);
        if( !objectp(girth) || base_name(girth) != WHITE_GIRTH )
            girth = carry_object(SILVER_GIRTH);
        if( objectp(girth) ) girth->wear();
    }
    carry_object("/custom/weapon/sword/sword_of_frost_edge")->wield();
    carry_object("/custom/weapon/dagger/dagger_of_frost_edge")->wield("secondhand dagger");

    if( clonep() ) call_out("wander", WANDER_INTERVAL);
}

// ------------------------------------------------------------------
// 走動：只走往五堂鎮內的出口；人在五堂鎮外就直接回交叉路口。
// ------------------------------------------------------------------

void wander()
{
    object env;
    mapping exits;
    string *dirs = ({});

    call_out("wander", WANDER_INTERVAL);

    if( !living(this_object()) || is_fighting() || is_busy() ) return;
    if( !objectp(env = environment()) ) return;

    if( strsrch(base_name(env), AREA_PREFIX) != 0 ) {
        message_vision(CYN + "$N身形一晃，轉眼便不見了蹤影。\n" NOR, this_object());
        move(HOME_ROOM);
        message_vision(CYN + "$N緩步走了過來。\n" NOR, this_object());
        return;
    }

    if( random(100) >= WANDER_CHANCE ) return;
    if( !mapp(exits = env->query("exits")) ) return;

    foreach( string dir, mixed dest in exits )
        if( stringp(dest) && strsrch(dest, AREA_PREFIX) == 0 )
            dirs += ({ dir });
    if( sizeof(dirs) )
        GO_CMD->main(this_object(), dirs[random(sizeof(dirs))]);
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
        command("say 閣下的志向相當明確，又何必來糾纏老朽呢？");
        return 0;
    }
    if( member_array(me->query_race(), ({ "ashura", "malik", "rainner" })) != -1 ) {
        command("say 閣下似乎不適合修習本門武功。");
        return 0;
    }
    return 1;
}

int init_apprentice(object me)
{
    if( ::init_apprentice(me) ) {
        me->set_class("fighter");
        me->set("title", "冷梅莊弟子");
        me->set("custom_faction", FACTION);
        do_chat(({
            "梅影風微微頷首，神情稍霽：好，從今日起，你便是我冷梅莊弟子。\n",
            "梅影風目光一凝，沉聲說道：入我門下，便當勤練不輟，莫要辱沒了冷梅莊的名聲。\n",
        }));
    }
    return 1;
}

// ------------------------------------------------------------------
// 傳授：冷梅莊弟子才教；每門只補足到剛好可以升到 1 級的經驗。
// ------------------------------------------------------------------

int acquire_skill(object me, string skill)
{
    if( me->query("custom_faction") != FACTION ) return 0;

    switch( skill ) {
    case "lunmay":
    case "mayin":
    case "seven":
    case "sword":
    case "secondhand sword":
    case "secondhand dagger":
    case "parry":
    case "dodge":
    case "unarmed":
        break;
    case "hainmay force":
    case "force":
        if( me->query_level() < 15 )
            return notify_fail("梅影風搖頭道：你根基尚淺，還承受不住本門的清冷真氣。\n");
        break;
    case "advance_lunmay":
        if( me->query_level() < 30 || me->query_skill("hainmay force", 1) < 100 )
            return notify_fail("梅影風搖頭道：你的寒梅心法尚未大成，傲梅暗劍訣還不是你能碰的。\n");
        break;
    default:
        return 0;
    }

    if( me->query_learn(skill) >= me->skill_threshold(skill, 1) )
        return notify_fail("梅影風淡淡說道：入門的要訣已經傳給你了，往後全看你自己。\n");

    me->improve_skill_exact(skill, me->skill_threshold(skill, 1) - me->query_learn(skill));
    tell_object(me, CYN + "梅影風將「" + to_chinese(skill) + "」的入門要訣傳授給你。\n" NOR);
    return 1;
}
