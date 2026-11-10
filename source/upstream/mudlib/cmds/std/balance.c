// balance.c -- 查詢錢莊存款
//
// 存款直接記在角色身上（"bank_account"，單位為文），所以任何地方都能查詢。
// 在錢莊裡查詢時，順便收回以前發出的舊金契。
inherit F_CLEAN_UP;

// 把以文為單位的金額寫成「幾錠金元寶幾兩黃金幾兩碎銀幾文錢」。錢莊也用這個格式。
string money_string(int amount)
{
    string str = "";

    if( amount < 0 ) return "賒欠" + money_string(-amount);
    // 金額最多寫到金元寶（使用者指示，2026-11-07）。
    if( amount >= 1000000 ) str += chinese_number(amount / 1000000) + "錠金元寶";
    if( amount % 1000000 >= 10000 ) str += chinese_number(amount % 1000000 / 10000) + "兩黃金";
    if( amount % 10000 >= 100 ) str += chinese_number(amount % 10000 / 100) + "兩碎銀";
    if( amount % 100 ) str += chinese_number(amount % 100) + "文錢";
    return str == "" ? "零文錢" : str;
}

int main(object me, string arg)
{
    object env = environment(me);

    if( env && function_exists("retire_bankbond", env) )
        env->retire_bankbond(me);
    // 沒有存款時不說「共有零文錢」（使用者指示，2026-10-25）。
    if( !me->query("bank_account") )
        write("你在錢莊並沒有任何存款。\n");
    else
        write("你在錢莊的存款共有" + money_string(me->query("bank_account")) + "。\n");
    return 1;
}

int help(object me)
{
    write(@HELP
指令格式：balance

查詢你在錢莊的存款。存款跟著你的角色，任何地方都能查詢。
在錢莊可以用 deposit <數量> <錢幣> 存款、withdraw <數量> <錢幣> 提款、
convert <數量> <錢幣> to <另類錢幣> 兌換，錢幣要用 id：
  coin（錢）  silver（碎銀）  gold（黃金）  sycee（金元寶）
  nugget（復刻紀念金元寶）
一錠金元寶值一百兩黃金，一錠復刻紀念金元寶值一百錠金元寶。
例如 deposit 25 coin、withdraw 1 sycee、convert 100 gold to sycee。
HELP);
    return 1;
}
