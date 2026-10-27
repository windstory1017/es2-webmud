#include <ansi.h>
#include <class_level.h>

inherit F_DBASE;

// Player stat setters require the class daemon to have its own effective uid.
static void create() { seteuid(getuid()); }

string query_rank(object ob, string politeness)
{
    if (!politeness) return "和尚";

    switch (politeness) {
        case "self":
            return "貧僧";
        case "respectful":
            return "大師";
        case "rude":
        default:
            return "禿驢";
    }
}

// 升級門檻（x 種族基數）：江湖歷練 (lv-1)^2x100、佛學修為 (lv-1)^2x100、
// 禪定修養 (lv-10)^2x100、文書能力 (lv-10)^2x100、聲望 (lv-30)^2x100。
void set_next_target(object ob, int cur_lv)
{
    int lv, coef;

    if (cur_lv < 1) cur_lv = 1;
    lv = cur_lv + 1;
    coef = race_coef(ob);

    ob->set_target_score("survive", sq_from(lv, 1) * 100 * coef / 100);
    ob->set_target_score("buddhology", sq_from(lv, 1) * 100 * coef / 100);
    ob->set_target_score("cultivation", sq_from(lv, 10) * 100 * coef / 100);
    ob->set_target_score("literature", sq_from(lv, 10) * 100 * coef / 100);
    ob->set_target_score("reputation", sq_from(lv, 30) * 100 * coef / 100);
}

void initialize(object ob)
{
    set_next_target(ob, ob->query_level());
}

void advance_level(object ob)
{
    grow_stats(ob, 6, 6, 6);
    // advance_level() 在等級加 1 之前執行，新等級是 query_level() + 1。
    set_next_target(ob, ob->query_level() + 1);
}
