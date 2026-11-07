/* CUSTOM RACE COMMAND: rainner feed (以鮮血餵養小蛇) */
#include <ansi.h>
inherit F_CLEAN_UP;

#define MAX_SPOTS 120

private void create() { seteuid(getuid()); }

// 顏色 : ({ 精門檻與消耗的斑點倍數, 常數, 氣, 神 })
// 白蛇的精門檻固定為 >1，消耗仍為 斑點x2+5。
private mapping feed_rule = ([
    "white":  ({ 2, 5, 10, 10 }),
    "black":  ({ 3, 7, 10, 10 }),
    "green":  ({ 2, 7, 15, 5 }),
    "red":    ({ 2, 8, 10, 5 }),
    "yellow": ({ 2, 8, 10, 5 }),
]);

int main(object me, string arg) {
    object snake;
    string color;
    int spots, gin_cost, gin_need, exp, gain;
    mixed *rule;

    if (me->query_race() != "rainner") return notify_fail("你不是雨師妾，無法餵養小蛇。\n");
    if (me->is_busy()) return notify_fail("你正忙著。\n");
    if (!arg || !objectp(snake = present(arg, me)) || !(color = snake->query("rainner_snake")))
        return notify_fail("你要餵養哪一條蛇？\n");
    if (snake->query_owner_id() != me->query("id"))
        return notify_fail(snake->name() + "不是你養的。\n");

    spots = snake->query_spots();
    if (spots >= MAX_SPOTS)
        return notify_fail(snake->name() + "的斑點已經長滿，成長已達極限了。\n");

    rule = feed_rule[color];
    gin_cost = spots * rule[0] + rule[1];
    gin_need = color == "white" ? 1 : gin_cost;
    if (me->query_stat("gin") <= gin_need || me->query_stat("kee") <= rule[2]
    ||  me->query_stat("sen") <= rule[3])
        return notify_fail(sprintf("你的精氣神不足以餵養%s（需要 精>%d、氣>%d、神>%d）。\n",
            snake->name(), gin_need, rule[2], rule[3]));

    // 目前值不夠扣時照樣餵，扣到底就會昏倒。
    me->consume_stat("gin", gin_cost, me);
    me->consume_stat("kee", rule[2], me);
    me->consume_stat("sen", rule[3], me);

    gain = 20 + random(spots);
    exp = me->query("rainner/snake/" + color) + gain;
    me->set("rainner/snake/" + color, exp);
    message_vision(HIR "$N咬破指尖，讓" + snake->name() + HIR "吸吮自己的鮮血。\n" NOR, me);

    if (snake->query_spots() > spots) {
        snake->refresh();
        tell_object(me, HIG + snake->name() + HIG "身上多了一個斑點，現在共有 "
            + snake->query_spots() + " 個斑點了！\n" NOR);
    }
    me->start_busy(1);
    return 1;
}

int help(object me) {
    write(
        "指令格式：feed <蛇>\n"
        "雨師妾以自己的鮮血餵養小蛇。每餵一口小蛇得到 20+random(斑點) 點經驗，\n"
        "累計經驗達到 n×n×10 時長出第 n 個斑點，最多 120 個斑點；能力依斑點線性成長。\n"
        "  白蛇 white viper   精>1      氣>10 神>10  消耗 精 斑點x2+5、氣 10、神 10\n"
        "  黑蛇 black viper   精>斑點x3+7 氣>10 神>10  消耗 精 斑點x3+7、氣 10、神 10\n"
        "  青蛇 green viper   精>斑點x2+7 氣>15 神>5   消耗 精 斑點x2+7、氣 15、神 5\n"
        "  赤蛇 red viper     精>斑點x2+8 氣>10 神>5   消耗 精 斑點x2+8、氣 10、神 5\n"
        "  黃蛇 yellow viper  精>斑點x2+8 氣>10 神>5   消耗 精 斑點x2+8、氣 10、神 5\n"
        "精的目前值不夠扣時仍可餵養，扣到底會昏倒。餵養後 busy 1 tick，沒有冷卻。\n"
    );
    return 1;
}
