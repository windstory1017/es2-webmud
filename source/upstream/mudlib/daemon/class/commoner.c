#include <ansi.h>
#include <class_level.h>

inherit F_DBASE;

// Player stat setters require the class daemon to have its own effective uid.
static void create() { seteuid(getuid()); }

string
query_rank (object obj, string politness)
{
	if (!politness)
		return "平民";

	switch (politness) {
		case "self":
			return "我是";
		case "respectful":
			return "朋友";
		case "rude":
		default:
			return "傢伙";
	}
}

// 平民升級（使用者定案，2026-10-20）：只看江湖歷練，門檻 (lv-1)^2x100 x 種族基數；
// 等級上限不分種族一律 50（見 feature/char/score.c 的 at_level_cap()）；
// 升級時精氣神不成長。拜師入行後保留目前等級。
// cur_lv 是目前等級，門檻是下一級 cur_lv+1 的。
void set_next_target(object ob, int cur_lv)
{
    if (cur_lv < 1) cur_lv = 1;
    ob->set_target_score("survive", sq_from(cur_lv + 1, 1) * 100 * race_coef(ob) / 100);
}

void initialize(object ob)
{
    set_next_target(ob, ob->query_level());
}

void advance_level(object ob)
{
    // advance_level() 在等級加 1 之前執行，新等級是 query_level() + 1。
    set_next_target(ob, ob->query_level() + 1);
}
