#include <ansi.h>
#include <class_level.h>

inherit F_DBASE;

// Player stat setters require the class daemon to have its own effective uid.
static void create() { seteuid(getuid()); }

string
query_rank (object obj, string politness)
{
	if (!politness)
		return "武者";

	switch (politness) {
		case "self":
			return "在下";
		case "respectful":
			return "英雄";
		case "rude":
		default:
			return "傢伙";
	}
}

// 升級門檻（x 種族基數）：實戰經驗 (lv-1)^2x150、武術造詣 (lv-1)^2x150、
// 武學之道 (lv-10)^2x100。cur_lv 是目前等級，門檻是下一級 cur_lv+1 的。
void set_next_target(object ob, int cur_lv)
{
    int lv, coef;

    if (cur_lv < 1) cur_lv = 1;
    lv = cur_lv + 1;
    coef = race_coef(ob);

    ob->set_target_score("combat", sq_from(lv, 1) * 150 * coef / 100);
    ob->set_target_score("martial art", sq_from(lv, 1) * 150 * coef / 100);
    ob->set_target_score("martial mastery", sq_from(lv, 10) * 100 * coef / 100);
}

void initialize(object ob)
{
    set_next_target(ob, ob->query_level());
}

void advance_level(object ob)
{
    grow_stats(ob, 4, 2, 8);
    // advance_level() 在等級加 1 之前執行，新等級是 query_level() + 1。
    set_next_target(ob, ob->query_level() + 1);
}
