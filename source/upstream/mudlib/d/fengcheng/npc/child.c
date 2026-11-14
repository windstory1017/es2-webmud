// /d/fengcheng/npc/child.c — 風城 NPC：字謎小孩（L18 小巷）。
//
// 猜字謎（舊資料「八寶樓謎語」的十題）：向小孩問起字謎，他隨機出一題「你說說看…是甚麼字？」，
// 玩家用 answer 回答；答對就記下 quest/fengcheng_riddle，之後可以通過將軍府門衛。
// 一次只出一題；答錯可以重答，也可以再問一次換題。

#include <npc.h>

inherit F_VILLAGER;

// ({ 謎面, 答案 })，謎面與答案都來自舊資料。
private mixed *RIDDLES = ({
    ({ "點點螢火江邊照", "淡" }),
    ({ "飛砂走石", "少" }),
    ({ "弓上一枝箭，箭邊一把弓", "引" }),
    ({ "手無寸鐵", "控" }),
    ({ "半個月亮", "胖" }),
    ({ "半真半假", "值" }),
    ({ "一點不見", "視" }),
    ({ "十個哥哥", "克" }),
    ({ "日落半林中", "東" }),
    ({ "一月七日", "脂" }),
});

void create()
{
    seteuid(getuid());
    set_name("小孩", ({ "child" }));
    set_race("human");
    set_class("commoner");
    set_level(1);
    set("gender", "male");
    set("age", 8);
    set("long", "一個蹲在牆根的小孩，手裡捏著一截石灰，眼睛骨碌碌地轉，一看就是滿肚子鬼點子。\n");
    set_skill("unarmed", 2);
    set_skill("dodge", 10);
    set_skill("parry", 2);
    set_power("C");
    setup();
}

void init()
{
    ::init();
    add_action("do_answer", "answer");
}

void say_to(object who, string msg)
{
    if( !objectp(who) || environment(who) != environment() ) return;
    command("say " + msg);
}

// 向小孩問起字謎（ask child about 字謎／謎）。
void accept_inquiry(object who, string key)
{
    int n;

    if( key != "字謎" && key != "謎" && key != "猜謎" ) return;
    if( who->query("quest/fengcheng_riddle") ) {
        command("emote 笑嘻嘻地拍了拍手上的石灰");
        command("say " + who->name() + "已經猜對過我的字謎了，這回我可沒有新的了。");
        return;
    }
    n = random(sizeof(RIDDLES));
    who->set_temp("fengcheng_riddle/answer", RIDDLES[n][1]);
    command("emote 眼睛一亮，把手裡的石灰一丟");
    call_out("say_to", 2, who, who->name() + "，你說說看" + RIDDLES[n][0] + "是甚麼字？");
}

int do_answer(string arg)
{
    object who = this_player();
    string right = who->query_temp("fengcheng_riddle/answer");

    if( !stringp(right) ) return 0;
    if( !arg || arg == "" ) return notify_fail("你要回答什麼？\n");
    message_vision("$N說道﹕" + arg + "。\n", who);
    if( arg == right ) {
        who->delete_temp("fengcheng_riddle/answer");
        who->set("quest/fengcheng_riddle", 1);
        command("emote 瞪大了眼睛，拍手笑了起來");
        command("say 答對了答對了！" + who->name() + "真聰明，將軍府的門衛見了你，想必不會再為難了。");
    } else {
        command("emote 搖頭晃腦地笑了起來");
        command("say 不對不對，再想想看。");
    }
    return 1;
}
