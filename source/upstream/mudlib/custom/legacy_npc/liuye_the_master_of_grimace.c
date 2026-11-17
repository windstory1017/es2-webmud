// /custom/legacy_npc/liuye_the_master_of_grimace.c — 舊站 NPC：劉乙忘玄（Liuye the Master of Grimace）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。2026-11-17 起在亂葬崗老和尚處（d/graveyard/grave7）出生，會在亂葬崗內走動。
舊站區域：哭笑門、青邪宮；房間：青邪宮哭笑門鎮心。
*/
#include <ansi.h>
#include <command.h>
inherit "/custom/legacy_npc/legacy_npc";

#define FACTION "fighter.huge"

void create()
{
    seteuid(getuid());
    set_name("劉乙忘玄", ({ "liuye the master of grimace", "liuye", "grimace", "master" }));
    set("title", "哭笑門門主");
    set("nickname", "哭笑野人");
    set_race("yenhold");
    set_class("fighter");
    set_level(70);
    set("gender", "male");
    set("age", 65);
    set("long",
        "這位兩眼如鷹, 滿臉鬚髦的八尺大漢, 正是青邪宮四門中位於首座的哭笑門\n"
        "門主劉乙忘玄。劉乙忘玄是青邪宮殘蟻師太的大弟子, 武藝超群, 為人極為\n"
        "深沉內斂, 年輕時膂力驚人, 身形魁梧, 一手青邪杖法打遍難逢敵手, 在當\n"
        "時在江湖上便得了個「降天山」的外號。\n"
        "劉乙忘玄中年時青邪宮發生重大變故, 青邪宮四門中的擾心門門主梵雲飛,\n"
        "和吹雪門門主王雙兒雙雙失蹤, 當時參與青邪宮「封鬼」盛會的武林豪傑死\n"
        "傷更是慘重。劉乙忘玄在此事過後, 突然性情大變, 整天又哭又笑, 意態狂\n"
        "亂, 狀如瘋癲。五十三歲那年, 殘蟻逝世, 劉乙忘玄瘋病竟然好轉, 自號「\n"
        "哭笑野人」, 成為青邪宮繼殘蟻師太之後的霸主, 之後潛心閉關五年, 創出\n"
        "「大邪心法」和「大邪七損刀」, 儼然成為武林中人皆敬佩的武學大宗師。\n");
    set("custom_faction", FACTION);   // 哭笑門（docs/martial_arts/門派武功對照表.md）
    // 武功見 docs/martial_arts/哭笑門_青邪杖法.md、哭笑門_大邪心法與其他武功.md。
    // 技能等級與杖法對應由門派表（daemon/misc/npc_skill.c）自動套用，這裡只補招架。
    map_skill("parry", "chin_staff");
    set("legacy_patrol", 1);   // 巡邏範圍見下面的 patrol()（使用者指示，2026-11-17）
    set_power("S");
    setup();
    carry_object("/custom/armor/feet/flying_boots")->wear();   // 飛天靴
    carry_object("/custom/armor/waist/ghosts_girdle")->wear();   // 百鬼腰束
    carry_object("/custom/weapon/staff/god_cane")->wield();   // 九曲十彎
    carry_object("/custom/armor/cloth/soft_cloth")->wear();   // 軟麻青絲衣
    carry_object("/custom/armor/neck/copper_rosary")->wear();   // 黃銅佛珠
}

// ------------------------------------------------------------------
// 拜師與傳授（使用者指示，2026-11-17：哭笑門師傅，主要武功青邪杖法）
// ------------------------------------------------------------------

int accept_info(object ob, string type)
{
    return type == "skills";
}

/* 拜師條件：平民（尚未加入任何職業）。拜師後成為武者（哭笑門弟子）。 */
int accept_apprentice(object me)
{
    if( me->query_class() != "commoner" ) {
        command("say 你已另有所屬，不必來我這兒。");
        return 0;
    }
    return 1;
}

int init_apprentice(object me)
{
    if( ::init_apprentice(me) ) {
        me->set_class("fighter");
        me->set("title", "哭笑門弟子");
        me->set("custom_faction", FACTION);
        command("emote 先是仰天大笑，繼而又眼眶泛紅，好一會兒才止住。");
        command("say 好，從今日起，你就是哭笑門的人了。");
    }
    return 1;
}

/* 傳授：哭笑門弟子才教；只補足到剛好可以升到 1 級的經驗，之後要自己修練。
 * 大邪心經是解謎取得，不在這裡傳授。 */
int acquire_skill(object me, string skill)
{
    if( me->query("custom_faction") != FACTION ) return 0;

    switch( skill ) {
    case "chin_staff":
    case "chitensword":
    case "hesanzhang":
    case "chin-steps":
    case "sword":
    case "staff":
    case "twohanded staff":
    case "parry":
    case "dodge":
    case "unarmed":
        break;
    case "huge force":
    case "force":
        if( me->query_level() < 11 )
            return notify_fail("劉乙忘玄搖頭道：你根基太淺，吃不下這路霸道真氣。\n");
        break;
    default:
        return 0;
    }

    if( me->query_learn(skill) >= me->skill_threshold(skill, 1) )
        return notify_fail("劉乙忘玄擺手道：入門的要訣已經給你了，剩下的自己去磨。\n");

    me->improve_skill_exact(skill, me->skill_threshold(skill, 1) - me->query_learn(skill));
    tell_object(me, CYN + "劉乙忘玄將「" + to_chinese(skill) + "」的入門要訣傳授給你。\n" NOR);
    return 1;
}

// ------------------------------------------------------------------
// 巡邏：可以在亂葬崗與太乙觀之間走動，不能走進雪亭鎮（使用者指示，2026-11-17）。
// 規則與 legacy_npc.c 的 patrol() 相同（每 4 秒 10% 機會移動），只是可走的區域不同。
// ------------------------------------------------------------------

#define PATROL_AREAS ({ "亂葬崗", "太乙觀" })

void patrol()
{
    object env, room;
    mapping exits;
    string *dirs = ({});

    call_out("patrol", 4);

    if( !living(this_object()) || is_fighting() || is_busy() ) return;
    if( !objectp(env = environment()) ) return;
    if( random(100) >= 10 ) return;
    if( !mapp(exits = env->query("exits")) ) return;

    foreach( string dir, mixed dest in exits ) {
        if( !stringp(dest) ) continue;
        if( !objectp(room = find_object(dest)) && catch(room = load_object(dest)) ) continue;
        if( objectp(room) && member_array(room->query("map/area"), PATROL_AREAS) != -1 )
            dirs += ({ dir });
    }
    if( sizeof(dirs) )
        GO_CMD->main(this_object(), dirs[random(sizeof(dirs))]);
}
