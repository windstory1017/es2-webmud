// class_level.h
//
// Shared helpers for class daemons: character level-up thresholds scaled by
// the race 基數, and the per-level gin/kee/sen growth.

// Race 基數 as a percentage (100 = human baseline), read from the race
// daemon's "commoner_score_base"; only scales character level-up thresholds.
private int race_coef(object ob)
{
    int coef;
    coef = RACE_D(ob->query_race())->query("commoner_score_base");
    return coef > 0 ? coef : 100;
}

// (lv-base)^2，lv 未超過 base 時為 0。
private int sq_from(int lv, int base)
{
    return lv > base ? (lv - base) * (lv - base) : 0;
}

// (lv-base)，lv 未超過 base 時為 0。
private int lin_from(int lv, int base)
{
    return lv > base ? lv - base : 0;
}

// 升級增加精氣神最大值：精 = 機敏/dex_div、氣 = 根骨/con_div、神 = 靈性/spi_div，
// 各自再隨機 +1、+0 或 -1，不會倒扣。用先天屬性計算。
private void grow_stats(object ob, int dex_div, int con_div, int spi_div)
{
    int gin_gain, kee_gain, sen_gain;

    gin_gain = ob->query_attr("dex", 1) / dex_div + random(3) - 1;
    kee_gain = ob->query_attr("con", 1) / con_div + random(3) - 1;
    sen_gain = ob->query_attr("spi", 1) / spi_div + random(3) - 1;
    if (gin_gain < 0) gin_gain = 0;
    if (kee_gain < 0) kee_gain = 0;
    if (sen_gain < 0) sen_gain = 0;

    ob->set_stat_maximum("gin", ob->query_stat_maximum("gin") + gin_gain);
    ob->set_stat_maximum("kee", ob->query_stat_maximum("kee") + kee_gain);
    ob->set_stat_maximum("sen", ob->query_stat_maximum("sen") + sen_gain);
}
