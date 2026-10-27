#include <ansi.h>
#include <class_level.h>

inherit F_DBASE;

// Player stat setters require the class daemon to have its own effective uid.
static void create() { seteuid(getuid()); }

string query_rank(object ob, string politeness)
{
    if (!politeness) return "方士";

    switch (politeness) {
        case "self":
            return "在下";
        case "respectful":
            return "仙師";
        case "rude":
        default:
            return "術士";
    }
}

// 升級門檻（x 種族基數）：江湖歷練 (lv-1)^2x100、法術道行 (lv-16)^2x100、
// 丹道修養 (lv-1)^2x100、法術修為 (lv-6)^2x100。
void set_next_target(object ob, int cur_lv)
{
    int lv, coef;

    if (cur_lv < 1) cur_lv = 1;
    lv = cur_lv + 1;
    coef = race_coef(ob);

    ob->set_target_score("survive", sq_from(lv, 1) * 100 * coef / 100);
    ob->set_target_score("magic mastery", sq_from(lv, 16) * 100 * coef / 100);
    ob->set_target_score("alchemy", sq_from(lv, 1) * 100 * coef / 100);
    ob->set_target_score("magic", sq_from(lv, 6) * 100 * coef / 100);
}

void initialize(object ob)
{
    set_next_target(ob, ob->query_level());
}

void advance_level(object ob)
{
    grow_stats(ob, 4, 4, 4);
    // advance_level() 在等級加 1 之前執行，新等級是 query_level() + 1。
    set_next_target(ob, ob->query_level() + 1);
}
