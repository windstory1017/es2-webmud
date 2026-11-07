// /d/wutang/npc/yaesae.c — 五堂鎮 NPC：玉面好漢 葉翔（依五堂鎮設計表）。

// 蝶影幻步任務（使用者提供舊資料，2026-11-07；設計見 docs/martial_arts/瑯夷派_蝶影幻步.md）：
// 玩家自己取得、交來飲血，葉翔道謝；盜賊（還沒學過）傳授蝶影幻步，其他人給 10 兩黃金。
// 任務不限次數；已學過的盜賊也領 10 兩黃金。葉翔戰鬥中不必真的施展蝶影迷蹤步。

#include <npc.h>
#include "/d/wutang/butterfly_quest.h"
inherit F_FIGHTER;

void create()
{
    seteuid(getuid());
    set_name("葉翔", ({ "yaesae", "ye xiang", "ye" }));
    set_race("human");
    set_class("thief");
    set_level(30);
    set("title", "玉面好漢");
    set("age", 30);
    set("long", @LONG
葉翔是武林中俠盜代表的佼佼者，時常劫富濟貧，是一個武林中豪
傑的代表，而他的容貌有如古代的潘安，所以被武林同道通稱為玉
面好漢，而他的輕功在武林中也是一絕。
LONG
    );
    set_skill("butterfly-steps", 110);
    // 約 5～7 tick 說一句（每 tick 16% 機率）。
    set("chat_chance", 15);
    set("chat_msg", ({
        "葉翔斜倚在公告欄旁，懶洋洋地打量著往來的富商，嘴角似笑非笑。\n",
        "葉翔低聲自語：「這鎮上有些人的銀子，也該拿出來分給窮苦人家了。」\n",
    }));
    set_power("A");
    setup();
}

void speak(object who, int i)
{
    string *lines = ({
        "$N高興的拿著飲血，小心的放在身上。",
        "$N「嗯」的一聲﹐一副欲言又止的樣子。",
    });

    // 玩家中途走開就不再說，但獎勵照給。
    if( !bq_here(this_object(), who) ) { call_out("reward", 0, who); return; }
    if( i < sizeof(lines) ) {
        bq_say(this_object(), who, lines[i]);
        call_out("speak", BQ_TICK, who, i + 1);
    } else {
        message_vision(CYN "$N對所有在場的人鞠躬道謝!\n" NOR, this_object());
        call_out("reward", BQ_TICK, who);
    }
}

void reward(object who)
{
    if( !objectp(who) ) return;
    who->delete(BQ_STAGE);
    who->delete(BQ_MEET);
    if( bq_here(this_object(), who) ) bq_say(this_object(), who, "$N點了點頭。");

    if( who->query_class() == "thief" && undefinedp(who->query_skills()["butterfly-steps"]) )
        call_out("teach", bq_here(this_object(), who) ? BQ_TICK : 0, who);
    else
        call_out("pay", bq_here(this_object(), who) ? BQ_TICK : 0, who);
}

void teach(object who)
{
    if( !objectp(who) ) return;
    if( bq_here(this_object(), who) )
        bq_say(this_object(), who,
            "$N說道﹕在下的輕功在武林是數一數二的，如果你不嫌棄，我就傅授給您吧。");
    who->improve_skill_exact("butterfly-steps", 1);
    tell_object(who, CYN "(葉翔將蝶影幻步的基本步法交給了你)\n" NOR);
}

// 不是盜賊、或已經學會的盜賊：給 10 兩黃金（玩家已走開就放在地上）。
void pay(object who)
{
    object gold;

    if( !objectp(who) ) return;
    if( bq_here(this_object(), who) )
        bq_say(this_object(), who, "$N說道﹕這是在下的一點心意，還請您不要推辭。");
    gold = new("/obj/money/gold");
    gold->set_amount(10);
    if( !bq_here(this_object(), who) || !gold->move(who) ) gold->move(environment(this_object()));
    else message_vision(CYN "$N給$n一些黃金。\n" NOR, this_object(), who);
}

// 收下玩家自己取得的飲血（別人轉手的不收）。
int accept_object(object who, object ob)
{
    if( !userp(who) || bq_stage(who) != BQ_BLADE || !bq_mine(who, ob, BQ_BLADE_FILE) ) return 0;
    call_out("speak", BQ_TICK, who, 0);
    return 1;
}
