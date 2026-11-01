// /custom/legacy_npc/wu_dauton.c — 舊站 NPC：武道通（Wu dauton）。
/*
LEGACY NPC. 名稱、英文 ID、敘述、年紀、性別、種族、裝備取自舊站畫面紀錄（DOCUMENTED，
docs/LEGACY_ROOM_NPC_COMPENDIUM.md）；職業、等級、強度、門派、行為依使用者提供的
「舊有 NPC 待製作清單」（2026-10-15）。
舊站區域：武陀灸堂；房間：武陀灸堂。武陀灸堂房間尚未製作，暫放雪亭鎮廣場北邊
（d/snow/square_n.c），在雪亭鎮巡邏（使用者決定，2026-10-27）。
武陀灸堂師父：收方士徒弟、傳授針灸之術與四套針法，
設計見 docs/martial_arts/武陀灸堂_門派設計.md 第二章。技能由 NPC 自動技能系統依門派給。
戰鬥行為（使用者定案，2026-10-28，同文件第九章）：開打即扎自己驚異霸甲針；
每 tick 40% 依敗膽一針到敗敏五針的順序扎向玩家；精氣神或 HP 低於兩成時刺針續力補滿，每條命一次。
*/
#include <ansi.h>

inherit "/custom/legacy_npc/legacy_npc";

#define FACTION "alchemist.wuto"

#define WUTO_CHANCE     40      // 每 tick 施展五敗針的機率（%）
#define REVIVE_RATIO    20      // 精氣神或 HP 低於上限的百分之幾時刺針續力
string *WUTO_ORDER = ({ "fear", "foul", "fail", "fling", "fumble" });   // 一針到五針

void create()
{
    seteuid(getuid());
    set_name("武道通", ({ "wu dauton", "wu", "dauton" }));
    set("title", "武陀灸堂堂主");
    set("nickname", "鬼華陀");
    set_race("human");
    set_class("alchemist");
    set_level(60);
    set("gender", "male");
    set("age", 75);
    set("long",
        "武道通年輕時本為龍圖丹派的方士, 因為一次因緣際會結識了一位江湖異人, 習得\n"
        "了幾手針灸之術, 從此便不再苦煉丹道, 開始如著魔般浸淫於施針下灸之學。之後\n"
        "竟因久疏龍圖丹派本門功夫, 而遭致開革之禍。武道通離開龍圖丹派之後, 便隱居\n"
        "於斐縣縣郊, 為當地居民針灸治病, 沒有想到卻聲名大噪起來, 由於醫術高明, 行\n"
        "事瘋癲乖張, 得了個「鬼華陀」的稱號。到了晚年, 乃自行開宗立派, 創設「武陀\n"
        "灸堂」, 傳授附近居民武藝與防身灸術, 門下弟子不下百人, 多雲遊四海, 行醫救\n"
        "世。\n");
    set("custom_faction", FACTION);
    set("legacy_patrol", 1);
    set_power("S");
    setup();
    carry_object("/custom/armor/feet/boots_of_cleanse")->wear();   // 塵不沾
    carry_object("/custom/weapon/needle/bee_needle")->wield();   // 蜂尾針
    carry_object("/custom/armor/finger/tiger_ring")->wear();   // 虎紋戒指
    carry_object("/custom/weapon/needle/needle_of_bone_maggot");   // 附骨之蛆
}

// ------------------------------------------------------------------
// 戰鬥行為：霸甲針在身（針力消退就再扎）；五敗針依序扎向玩家，扎中才換下一針；
// 精氣神或 HP 低於兩成時刺針強行續力，全部補滿，每條命一次。
// ------------------------------------------------------------------

private int low_stat(string what)
{
    int max = query_stat_maximum(what);
    return max > 0 && query_stat(what) * 100 < max * REVIVE_RATIO;
}

private void revive()
{
    object w = query_temp("weapon/needle");

    foreach( string what in ({ "gin", "kee", "sen", "HP" }) ) {
        if( query_stat_maximum(what) <= 0 ) continue;
        set_stat_effective(what, query_stat_maximum(what));
        set_stat_current(what, query_stat_maximum(what));
    }
    set_temp("wuto/revived", 1);
    message_vision(HIR "$N怪笑一聲，反手將" + (objectp(w) ? w->name() + HIR : "銀針")
        + "扎進自己胸前膻中穴，枯瘦的身軀猛然一挺竟又精神抖擻起來。\n" NOR,
        this_object(), 0, 1);
}

// 五敗針的對象：正在交手、同一個地方、還清醒的玩家。
private object wuto_target()
{
    object *list = filter(query_enemy(), (: objectp($1) && userp($1) && living($1)
        && environment($1) == environment() :));
    return sizeof(list) ? list[random(sizeof(list))] : 0;
}

static void heart_beat()
{
    object target;
    int step;

    ::heart_beat();
    if( !this_object() || !living(this_object()) || !is_fighting() ) return;

    if( !query_temp("wuto/revived")
    &&  (low_stat("gin") || low_stat("kee") || low_stat("sen") || low_stat("HP")) )
        revive();

    if( is_busy() ) return;
    if( !query_condition("wuto_amazing") ) {
        SKILL_D("amazing-needle")->do_amazing(this_object(), "armor", this_object());
        return;
    }
    if( random(100) >= WUTO_CHANCE || !objectp(target = wuto_target()) ) return;
    step = query_temp("wuto/step");
    if( SKILL_D("wuto-needle")->do_wuto(this_object(), WUTO_ORDER[step], target) )
        set_temp("wuto/step", (step + 1) % sizeof(WUTO_ORDER));
}

// M-04：須為平民（尚未入其他門派）才能拜師，與龍圖丹派互斥。
int accept_apprentice(object me)
{
    if( is_chatting() ) return 0;
    if( me->query("custom_faction") == FACTION ) return 1;
    if( me->query_class() != "commoner" || me->query("title") ) {
        do_chat(({
            "武道通斜眼瞧了瞧你，嘿嘿怪笑道﹕你身上早帶著別家的規矩，老夫這裡容不下。\n",
            "武道通揮了揮手，自顧自地低頭擦拭手中的銀針。\n",
        }));
        return 0;
    }
    return 1;
}

int init_apprentice(object me)
{
    if( me->query("custom_faction") == FACTION ) return ::init_apprentice(me);
    if( !::init_apprentice(me) ) return 0;
    me->set_class("alchemist");
    me->set("custom_faction", FACTION);
    me->set("title", "武陀灸堂弟子");
    do_chat(({
        CYN "武道通拈著鬍子嘿嘿一笑﹕好，好！老夫門下又多一個不怕扎針的。\n" NOR,
        CYN "武道通說道﹕針灸這門學問，扎得準能救人，扎得狠能殺人，你自己掂量著用。\n" NOR,
    }));
    return 1;
}

// M-05：拜師後傳授，只教到經驗值 50，之後靠自己施針累積。
void teach_to_cap(object ob, string skill)
{
    ob->improve_skill_exact(skill, 50 - ob->query_learn(skill));
    if( ob->query_learn(skill) > 50 ) ob->set_learn(skill, 50);
}

int acquire_skill(object ob, string skill)
{
    if( is_chatting() )
        return notify_fail("你最好等他老人家的話說完再插嘴 ...。\n");
    if( !ob->is_apprentice_of(this_object()) ) return 0;

    switch(skill) {
    case "cauterization":
    case "amazing-needle":
    case "wuto-needle":
        break;
    case "potency-needle":
    case "recovery-needle":
        if( ob->query_skill("kee anatomy", 1) < 80 ) {
            command("say 經脈穴道都還認不全，就想學這個？先回去把穴道背熟了再來。");
            return 1;
        }
        break;
    default:
        return 0;
    }
    if( ob->query_learn(skill) >= 50 ) {
        command("say 入門的手法都教給你了，往後要扎得準，得靠你自己多下針。");
        return 1;
    }
    // 傳授敘述整句顯示、不自動換行（message_vision 第四個參數），比照 acquire 的求教訊息。
    message_vision(CYN "$N抓起$n的手腕，在$n臂上比劃著穴道，將" + to_chinese(skill)
        + "的入門手法一一傳授給$n。\n" NOR, this_object(), ob, 1);
    teach_to_cap(ob, skill);
    return 1;
}
