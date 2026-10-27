#include <ansi.h>
// teacher.c

#include <npc.h>

inherit F_VILLAGER;

string *literate_text = ({
    "人之初", "性本善",
    "性相近", "習相遠",
    "苟不教", "性乃遷",
    "教之道", "貴以專",
    "昔孟母", "擇鄰處",
    "子不學", "斷機杼",
    "竇燕山", "有義方",
    "教五子", "名俱揚",
    "養不教", "父之過",
    "教不嚴", "師之惰",
    "為人子", "方少時",
    "親師友", "習禮儀",
});

int text_cnt = 0;
object picked_student;

void teach_literate();
void pick_student(object ob);
void cancel_pick(object ob);

/* 成為書生的條件（使用者指定，2026-10-17）：平民、讀書識字 5～29。
 * 被挑中時與答應時各檢查一次。 */
int can_be_scholar(object ob)
{
    int lv;

    if( !ob || ob->query_class() != "commoner" ) return 0;
    lv = ob->query_skill("literate", 1);
    return lv >= 5 && lv <= 29;
}

void create()
{
    set_name("王懷芝", ({ "wang whaiz", "wang", "whaiz" }) );
    set_race("human"); set_class("scholar");
    set_level(5);

    set("long",
        "王懷芝是鎮上王員外的獨子﹐前幾年上京考試落第﹐碰巧王太夫人過世\n"
        "﹐因此留在家鄉服喪﹐閒暇無事﹐便開了間私塾教鎮上孩子讀書。\n");
    set("chat_chance", 40);
    set("chat_msg", ({
        (: teach_literate :)
    }));
    set_power("B");
    setup();
}

void init()
{
    /* 玩家走進私塾時取消原本的聽課狀態：離開再回來要重新坐下。 */
    if( userp(this_player()) )
        this_player()->delete_temp("pending/sit_learning_literate");
    add_action("do_sit", "sit");
}

int do_sit(string arg)
{
    // add by dragoon
    if( this_player()->query_skill("literate", 1) >= 30 )
	return notify_fail("你的讀書識字根基不錯，不需要再聽三字經了。\n");
	
    if( this_player()->query_temp("pending/sit_learning_literate") )
        return notify_fail("你已經坐著專心聽講了。\n");
    message_vision(CYN + "$N找了一個空位坐了下來﹐開始跟著王老師唸書。\n" NOR,
        this_player() );
    this_player()->set_temp("pending/sit_learning_literate", 1);
    return 1;
}

void teach_literate()
{
    object *students, ob;
    string txt;

    if( text_cnt >= sizeof(literate_text) ) {
        command("say 嗯 ... 一次教太多你們也記不住﹐好吧﹐我們再從頭複習一次。\n");
        text_cnt = 0;
    }
    txt = literate_text[text_cnt++];
    command("say 大家跟著我念﹕" + txt + "。\n");

    students = all_inventory(environment());
    foreach(ob in students)
    {
	if( !ob->query_temp("pending/sit_learning_literate")
	||  !living(ob)
	||  ob->is_fighting()
	||  ob->query_skill("literate",1) >= 30 ) continue;

	/* 每句得到 智力 × random(智力) 的讀書識字點數。 */
	ob->improve_skill_exact("literate",
	    ob->query_attr("int") * random(ob->query_attr("int")));
	message_vision(CYN + "$N跟著念道﹕" + txt + "。\n" NOR, ob);

	/* 挑選書生的條件 */
	// 加個random, 不然每講一句就問 -Dragoon
	if( interactive(ob) && random(10) > 7 
	&&  can_be_scholar(ob)
	&&  !ob->query_temp("pending/picked_by_whaiz")
	&&  member_array(ob->query_race(), ({
		"avatar", "human", "rainner", "malik", "woochan", "jiaojao", "yaksa"
	    })) != -1
	&&  ob->query_attr("int") > random(20) )
	    do_chat( (: pick_student, ob :) );
    }
}

int accept_fight()
{
    do_chat("王懷芝說道﹕這 ...武功我可是半點不會﹐這位朋友不如去找我們家王教頭切磋切磋﹖\n");
    return 0;
}

string rank(string politeness, int raw)
{
    if( raw ) return ::rank(politeness, raw);
    switch(politeness) {
        case "self": return "小生";
        case "respectful": return "王老師";
        default:
            return ::rank(politeness, raw);
    }
}

int accept_info(object ob, string type)
{
    return type=="skills";
}

int acquire_skill(object who, string skill)
{
    if( skill=="literate" ) {
	command("say 想讀書識字嗎﹖在這裡坐著聽吧。\n");
	command("smile");
	command("say 讀書可不容易﹐不過學了多少一輩子都是你的。\n");
	return 1;
    }
    return 0;
}

void pick_student(object ob)
{
    if( ! (ob && present(ob) && interactive(ob)) ) return;

    command("say " + ob->name() + "，你在我這兒唸書有一段時間了。\n");
    command("say 我看你對讀書頗有興趣，天份也不錯。\n");
    command("say 想不想和老師一樣，當一個書生？\n");

    ob->set_temp("pending/picked_by_whaiz", 1);
    call_out( (: cancel_pick, ob :), 20);
}

void cancel_pick(object ob)
{
    if( ! ob ) return;
    /* 已經回答過了（pending 已清掉），不再多說。 */
    if( ! ob->query_temp("pending/picked_by_whaiz") ) return;

    ob->delete_temp("pending/picked_by_whaiz");
    if( present(ob) ) {
	command("pat " + ob->query("id"));
	command("say 也罷，人各有志。");
    }
}

void make_new_scholar(object ob)
{
    if( !ob || !present(ob) || !interactive(ob)) return;

    if( !can_be_scholar(ob) ) {
	command("say 罷了，這事以後再說吧。");
	return;
    }
    ob->set_class("scholar");
    command("smile");
    command("say 好好念書吧，我看你資質不錯，將來必有成就！");
    tell_object(ob, CYN + "( 你成為一位書生 )\n" NOR);
}

void relay_say(object who, string msg)
{
    string word;

    if( ! who->query_temp("pending/picked_by_whaiz") ) return;
    if( !stringp(msg) ) return;
    // 回答以「想」「是」「好」開頭就答應。中文字在 UTF-8 裡一個字佔 3 個位元組，
    // 原本只比對前 2 個位元組（舊 Big5 一字 2 位元組的寫法），所以永遠不會成立。
    foreach(word in ({ "想", "是", "好" })) {
	if( msg[0..strlen(word)-1] == word ) {
	    who->delete_temp("pending/picked_by_whaiz");
	    do_chat((: make_new_scholar, who :));
	    return;
	}
    }
}
