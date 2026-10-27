#include <ansi.h>
#include <class_level.h>

inherit F_DBASE;

// Player stat setters require the class daemon to have its own effective uid.
static void create() { seteuid(getuid()); }

string query_rank(object ob, string politeness)
{
    if (!politeness) return "書生";

    switch (politeness) {
        case "self":
            return "小生";
        case "respectful":
            return "公子";
        case "rude":
        default:
            return "書呆子";
    }
}

// 升級門檻（x 種族基數）：江湖歷練 (lv-1)^2x100、實戰經驗 (lv-1)^2x100、
// 文書能力 (lv-1)^2x50、文學造詣 (lv-10)^2x50、聲望 (lv-10)^2x50。
void set_next_target(object ob, int cur_lv)
{
    int lv, coef;

    if (cur_lv < 1) cur_lv = 1;
    lv = cur_lv + 1;
    coef = race_coef(ob);

    ob->set_target_score("survive", sq_from(lv, 1) * 100 * coef / 100);
    ob->set_target_score("combat", sq_from(lv, 1) * 100 * coef / 100);
    ob->set_target_score("literature", sq_from(lv, 1) * 50 * coef / 100);
    ob->set_target_score("literature mastery", sq_from(lv, 10) * 50 * coef / 100);
    ob->set_target_score("reputation", sq_from(lv, 10) * 50 * coef / 100);
}

void initialize(object ob)
{
    set_next_target(ob, ob->query_level());
}

void advance_level(object ob)
{
    grow_stats(ob, 5, 3, 3);
    // advance_level() 在等級加 1 之前執行，新等級是 query_level() + 1。
    set_next_target(ob, ob->query_level() + 1);
}
