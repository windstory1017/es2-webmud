// finance.c
// CUSTOM ECONOMY CONVENIENCE PATCH:
// Spending is value-based and automatically returns canonical change.
// This removes the need to visit a bank merely to break denominations.
//
// 金元寶（sycee）與復刻紀念金元寶（nugget）：付錢時由小到大拆換
// （使用者指示，2026-11-07，設計文件 docs/CURRENCY.md）。
// 1. 先用黃金、碎銀、文錢這些零錢付。
// 2. 零錢不夠，才拆金元寶，只拆不得不拆的錠數。
// 3. 金元寶也不夠，才把復刻紀念金元寶拆成一百錠金元寶，再照第 2 步拆。
// 找回的錢整理成黃金、碎銀、文錢；沒拆到的金元寶、紀念金元寶原封不動。

#define SYCEE_VALUE     1000000     // 一錠金元寶 = 一百兩黃金
#define NUGGET_SYCEE    100         // 一錠紀念金元寶 = 一百錠金元寶

private int query_money_amount(string type)
{
    object ob;

    ob = present(type + "_money", this_object());
    if (!ob || ob->query("money_id") != type) return 0;
    return (int)ob->query_amount();
}

// 黃金以下的零錢總值（文）。
private int query_small_money_value()
{
    object ob;
    int total;

    total = 0;
    ob = present("gold_money", this_object());
    if (ob) total += (int)ob->value();
    ob = present("silver_money", this_object());
    if (ob) total += (int)ob->value();
    ob = present("coin_money", this_object());
    if (ob) total += (int)ob->value();

    return total;
}

private void set_money_amount(string type, int amount)
{
    object ob;

    ob = present(type + "_money", this_object());

    if (amount < 1) {
        if (ob) ob->set_amount(0);
        return;
    }

    if (!ob) {
        ob = new("/obj/money/" + type);
        ob->set_amount(amount);
        if (!ob->move(this_object())) {
            destruct(ob);
            error("finance: unable to return money/change to character.\n");
        }
        return;
    }

    ob->set_amount(amount);
}

// 把零錢整理成黃金、碎銀、文錢（不會湊成金元寶）。
private void set_small_money_value(int value)
{
    int gold, silver, coin;

    if (value < 0) value = 0;

    gold = value / 10000;
    value %= 10000;
    silver = value / 100;
    coin = value % 100;

    set_money_amount("gold", gold);
    set_money_amount("silver", silver);
    set_money_amount("coin", coin);
}

varargs int
pay_money(int amount, int check_only)
{
    int small, sycee, nugget, need, n;

    if (amount < 1) return 1;

    small = query_small_money_value();
    sycee = query_money_amount("sycee");
    nugget = query_money_amount("nugget");

    if (small + sycee * SYCEE_VALUE + nugget * NUGGET_SYCEE * SYCEE_VALUE < amount)
        return 0;
    if (check_only) return 1;

    // 零錢不夠：先拆金元寶。
    if (small < amount && sycee > 0) {
        need = amount - small;
        n = (need + SYCEE_VALUE - 1) / SYCEE_VALUE;
        if (n > sycee) n = sycee;
        sycee -= n;
        small += n * SYCEE_VALUE;
    }

    // 還不夠：把紀念金元寶拆成金元寶，再拆需要的金元寶。
    if (small < amount && nugget > 0) {
        need = amount - small;
        n = (need + SYCEE_VALUE - 1) / SYCEE_VALUE;     // 還要拆幾錠金元寶
        nugget -= (n + NUGGET_SYCEE - 1) / NUGGET_SYCEE;
        sycee += ((n + NUGGET_SYCEE - 1) / NUGGET_SYCEE) * NUGGET_SYCEE - n;
        small += n * SYCEE_VALUE;
    }

    set_money_amount("nugget", nugget);
    set_money_amount("sycee", sycee);
    set_small_money_value(small - amount);

    return 1;
}

// can_afford()
//
// CUSTOM semantics:
// 0: cannot afford the price.
// 1: total carried money is enough; exact change is handled automatically.
// Legacy return value 2 ("needs smaller change") is intentionally retired.

int can_afford(int amount) { return pay_money(amount, 1); }
