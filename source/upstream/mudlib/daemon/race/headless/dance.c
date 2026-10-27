/* CUSTOM A-H RACE COMMAND: headless dance (戰舞) */
#include <ansi.h>
inherit F_CLEAN_UP;
#define DANCE_CONDITION "headless_dance"
#define RITUAL_CONDITION "headless_ritual"

private void create() { seteuid(getuid()); }

// 五種戰舞都要手持斧頭（原文「每揮舞一次斧頭」）。
int wielding_axe(object me) {
    mapping weapon = me->query_temp("weapon");
    if (!mapp(weapon)) return 0;
    return objectp(weapon["axe"]) || objectp(weapon["secondhand axe"])
        || objectp(weapon["twohanded axe"]);
}

private int ticks(int n) { return n < 1 ? 1 : n; }

private string usage() {
    return "戰舞格式：dance for <glory|fury|sorrow|axe|rite>\n";
}

int main(object me, string arg) {
    string mode;

    if (me->query_race() != "headless") return notify_fail("你不是刑天，無法使用戰舞。\n");
    if (me->is_fighting()) return notify_fail("戰鬥中無法使用戰舞。\n");
    if (me->is_busy()) return notify_fail("你正忙著。\n");
    if (!arg) return notify_fail(usage());
    if (!wielding_axe(me)) return notify_fail("你手中沒有斧頭，無法跳起戰舞。\n");

    mode = arg;
    if (strlen(mode) > 4 && mode[0..3] == "for ") mode = mode[4..];

    load_object("/custom/race/condition/" + DANCE_CONDITION);
    load_object("/custom/race/condition/" + RITUAL_CONDITION);

    switch (mode) {
    case "glory":
        me->set_condition(DANCE_CONDITION, ([
            "mode":"glory", "duration":ticks(me->query_attr("int") / 2),
            "damage_bonus":5 + me->query_attr("str") / 2,
            "armor_bonus":5 + me->query_attr("cps") / 2,
            "serial":time() * 1000 + random(1000)
        ]));
        message_vision(HIY "$N高舉斧頭，跳起莊嚴的榮光之舞，周身彷彿籠罩著一層光輝。\n" NOR, me);
        break;
    case "fury":
        me->set_condition(DANCE_CONDITION, ([
            "mode":"fury", "duration":ticks(me->query_attr("cor") / 2),
            "damage_bonus":15,
            "intimidate_bonus":me->query_attr("dex") / 2,
            "serial":time() * 1000 + random(1000)
        ]));
        message_vision(HIR "$N揮斧狂舞，跳起激昂的忿怒之舞，雙眼燃起熊熊戰意！\n" NOR, me);
        break;
    case "axe":
        me->set_condition(DANCE_CONDITION, ([
            "mode":"axe", "duration":ticks(me->query_attr("wis") / 2),
            "axe_bonus":me->query_attr("int") + 10,
            "serial":time() * 1000 + random(1000)
        ]));
        message_vision(HIW "$N斧隨身轉，跳起古老的斧舞，斧刃劃出一道道凌厲的弧光。\n" NOR, me);
        break;
    case "sorrow":
    case "rite":
        if (me->query_condition(RITUAL_CONDITION))
            return notify_fail("你正在跳舞中。\n");
        // 次數 = 慧根/2；下指令時先作用一次，之後每 tick 再作用一次。
        me->set_condition(RITUAL_CONDITION, ([
            "mode":mode, "duration":ticks(me->query_attr("wis") / 2),
            "room":environment(me),
            "serial":time() * 1000 + random(1000)
        ]));
        if (mode == "sorrow")
            message_vision(HIB "$N垂下斧頭，緩緩跳起哀傷之舞，舞姿沉痛如泣。\n" NOR, me);
        else
            message_vision(HIG "$N雙手捧斧，繞著圈子跳起祭舞，口中低吟古老的祭詞。\n" NOR, me);
        ("/custom/race/condition/" + RITUAL_CONDITION)->ritual_effect(me, mode);
        return 1;
    default:
        return notify_fail(usage());
    }

    me->start_busy(2);
    if (userp(me)) me->save();
    return 1;
}

// 說明改讀 help 檔案，和其他種族看到的 help dance 相同（使用者定案，2026-10-22）。
int help(object me) {
    string doc = read_file(HELP_DIR + "dance.txt");
    write(stringp(doc) ? doc : usage());
    return 1;
}
