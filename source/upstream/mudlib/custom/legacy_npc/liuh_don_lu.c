#include <ansi.h>
// /custom/legacy_npc/liuh_don_lu.c — 舊站 NPC：柳東蘆（Liuh don-lu）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。放在喬陰縣正廳（d/qiaoyin/fonxan_hall.c）。
封山派師父（使用者定案，2026-10-17）：平民可拜師，拜師後成為「封山派弟子」。
武功見 docs/martial_arts/封山派_封山劍法與倒亂七星步法.md。
戰鬥行為（使用者定案，2026-10-28）：凝神專注、使用未開放的封山劍法特殊招式、
氣或 HP 低於兩成時補滿（每次重生最多四次）。
*/
inherit "/custom/legacy_npc/legacy_npc";

#define FACTION "fighter.fonxan"

#define FULL_HEAL_MAX   4       // 補滿的次數上限（每次重生重新計算）
#define FULL_HEAL_RATIO 20      // 氣或 HP 低於上限的百分之幾時補滿

void create()
{
    seteuid(getuid());
    set_name("柳東蘆", ({ "liuh don-lu", "liuh", "don-lu" }));
    set("title", "封山派掌門");
    set_race("human");
    set_class("fighter");
    set_level(70);
    set("gender", "male");
    set("age", 55);
    set("long",
        "柳東蘆是赫赫有名的封山劍派掌門人，封山劍派原本只以劍法見長，但是柳東\n"
        "蘆近幾年以家傳拳法融合封山劍法劍意，彌補了封山劍法中的許多缺點，四年\n"
        "前的武林大會便技壓群雄奪得武林盟主的寶座，但是他當武林盟主不到半年便\n"
        "宣佈退隱，據說是因為身染絕症之故，不過在你面前的這位年約五、六十歲的\n"
        "白髮老者似乎仍然精神矍鑠，不像是身有重病的樣子。\n");
    // 武功等級依門派自動配置（daemon/misc/npc_skill.c）；玄蘇劍是雙手劍，另外指定雙手劍也用封山劍法。
    map_skill("twohanded sword", "fonxansword");
    set("fonxan/npc_specials", 1);   // 封山劍法未開放的特殊招式他照樣使用（fonxansword.c）

    set("custom_faction", FACTION);   // 封山派（docs/martial_arts/門派武功對照表.md）
    set_power("S");
    setup();
    carry_object("/custom/armor/leg/red_scale_leggings")->wear();   // 火鱗脛甲
    carry_object("/custom/armor/waist/red_scale_girth")->wear();   // 火鱗腰帶
    carry_object("/custom/armor/hand/red_scale_gloves")->wear();   // 火鱗手套
    carry_object("/custom/armor/head/red_scale_helm")->wear();   // 火鱗盔
    carry_object("/custom/armor/feet/red_scale_boots")->wear();   // 火鱗靴
    carry_object("/custom/armor/armor/red_scale_plate")->wear();   // 火鱗胸鎧
    carry_object("/custom/armor/cloth/red_scale_cloth")->wear();   // 火鱗衣
    carry_object("/custom/weapon/sword/blackthorn")->wield();   // 玄蘇劍
}

// ------------------------------------------------------------------
// 戰鬥行為：凝神專注（效果結束後再運）；氣或 HP 低於兩成時以封山真氣補滿，最多四次。
// ------------------------------------------------------------------

private int low_stat(string what)
{
    int max = query_stat_maximum(what);
    return max > 0 && query_stat(what) * 100 < max * FULL_HEAL_RATIO;
}

private void full_heal()
{
    foreach( string what in ({ "kee", "HP" }) ) {
        if( query_stat_maximum(what) <= 0 ) continue;
        set_stat_effective(what, query_stat_maximum(what));
        set_stat_current(what, query_stat_maximum(what));
    }
    add_temp("liuh/full_heal", 1);
    message_vision(HIC "$N一聲清嘯，精純的封山真氣灌注週身經脈，氣色頓時好了許多。\n" NOR, this_object());
}

static void heart_beat()
{
    ::heart_beat();
    if( !this_object() || !living(this_object()) || !is_fighting() ) return;

    if( query_temp("liuh/full_heal") < FULL_HEAL_MAX && (low_stat("kee") || low_stat("HP")) )
        full_heal();

    if( !query_temp("fonxan/focus") && !is_busy() )
        exert_function("focus");
}

// ------------------------------------------------------------------
// 拜師：平民可拜師，拜師後成為武者、封山派弟子。
// ------------------------------------------------------------------

int accept_info(object ob, string type)
{
    return type == "skills";
}

int accept_apprentice(object me)
{
    if( me->query_class() != "commoner" ) {
        command("say 閣下已有師承，柳某不敢奪人之美。");
        return 0;
    }
    return 1;
}

int init_apprentice(object me)
{
    if( ::init_apprentice(me) ) {
        me->set_class("fighter");
        me->set("title", "封山派弟子");
        me->set("custom_faction", FACTION);
        do_chat(({
            "柳東蘆撫鬚微笑：好，從今日起，你便是我封山派弟子。\n",
            "柳東蘆說道：封山劍法不求一擊制敵，貴在綿長不絕，你要記住了。\n",
        }));
    }
    return 1;
}

// ------------------------------------------------------------------
// 傳授：封山派弟子才教；每門只補足到剛好可以升到 1 級的經驗。
// ------------------------------------------------------------------

int acquire_skill(object me, string skill)
{
    if( me->query("custom_faction") != FACTION ) return 0;

    switch( skill ) {
    case "fonxansword":
    case "chaos-steps":
    case "sword":
    case "twohanded sword":
    case "parry":
    case "dodge":
    case "unarmed":
        break;
    case "fonxan force":
    case "force":
        if( me->query_level() < 15 )
            return notify_fail("柳東蘆搖頭道：你根基尚淺，還承受不住本門的醇厚真氣。\n");
        break;
    default:
        return 0;
    }

    if( me->query_learn(skill) >= me->skill_threshold(skill, 1) )
        return notify_fail("柳東蘆說道：入門的要訣已經傳給你了，往後全看你自己。\n");

    me->improve_skill_exact(skill, me->skill_threshold(skill, 1) - me->query_learn(skill));
    tell_object(me, CYN + "柳東蘆將「" + to_chinese(skill) + "」的入門要訣傳授給你。\n" NOR);
    return 1;
}

// ------------------------------------------------------------------
// 水雲劍法精義（使用者定案，2026-10-20）：水雲劍法 17 級以上、劍術 enable 水雲劍法，
// 找柳東蘆比武並輸給他，他會演示自己整理過的精義，玩家直接學會（只發生一次）。
// ------------------------------------------------------------------

void win_fight(object ob)
{
    if( !objectp(ob) || !userp(ob) ) return;
    if( ob->query("sayuensword/essence") ) return;
    if( ob->query_skill("sayuensword", 1) < 17 ) return;
    if( ob->skill_mapped("sword") != "sayuensword" ) return;
    call_out("teach_essence", 1, ob, 0);
}

// 每行控制在一行內（約 36 字），依語氣斷句，整句顯示不自動換行。
private void say_lines(object ob, string *lines)
{
    foreach(string line in lines)
        message_vision(CYN + line + "\n" NOR, this_object(), ob, 1);
}

void teach_essence(object ob, int step)
{
    string who;

    if( !objectp(ob) || environment(ob) != environment() ) return;
    who = "這位" + ob->rank("respectful");
    switch( step ) {
    case 0:
        say_lines(ob, ({
            "$N收劍而立﹐望著$n方才的劍路﹐輕輕嘆了口氣。",
            "$N說道﹕這套劍法﹐是小女閒來胡亂拼湊的玩意兒。",
            "$N又道﹕老夫只當她孩子心性﹐由著她胡鬧﹐不想竟誤了" + who + "的功夫。",
        }));
        break;
    case 1:
        say_lines(ob, ({
            "$N說道﹕這是老夫重新整理的水雲劍法精義﹐就當與" + who + "切磋一場罷。",
        }));
        break;
    default:
        say_lines(ob, ({
            // 一整句；超過一行時在逗號處手動換行。
            "$N長劍出鞘﹐將水雲劍法從頭到尾演示了一遍﹐\n" CYN "招式與小女孩所使一般無二﹐劍意卻已截然不同。",
        }));
        ob->set("sayuensword/essence", 1);
        ob->improve_skill_exact("sayuensword essence",
            ob->skill_threshold("sayuensword essence", 1) - ob->query_learn("sayuensword essence"));
        tell_object(ob, CYN + "你掌握了水雲劍法的精義。\n" NOR);
        return;
    }
    call_out("teach_essence", 2, ob, step + 1);
}
