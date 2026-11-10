/* CUSTOM RACE COMMAND: yaksa devour */
#include <ansi.h>
inherit F_CLEAN_UP;

#define TICK_SECONDS 2

// 吞魂封印開關：2026-11-03 封印，2026-11-04 改版後重新開放（使用者指示）。
// 要再封印時把 DEVOUR_SEALED 改成 1，再 update 本檔即可。
#define DEVOUR_SEALED 0

private void create() { seteuid(getuid()); }

// 吞食期間以 call_out 每 tick 吞一口，保持載入。
int clean_up() { return 1; }

private int rnd(int n) { return n > 0 ? random(n) : 0; }

private void stop_devour(object me) {
    if (objectp(me)) me->delete_temp("yaksa_devour");
}

// 永久增加精氣神最大值，並同時補上目前值，讓增加的部分立即可用。
private void grow(object me, string stat, int n) {
    if (n < 1) return;
    me->advance_stat(stat, n);
    me->heal_stat(stat, n);
    me->supplement_stat(stat, n);
}

// 有效吞魂次數（使用者指示，2026-11-04 歸零）：只有達到門檻的吞魂才計數。
// 舊規則的次數 yaksa/devoured_players 不再使用。
#define COUNT_KEY "yaksa/devour_count"

// 吞魂紀錄（使用者指示，2026-11-04）。本檔的身分（Mudlib）不能寫 /log，
// 系統權限只開放 /data/daemon 給 Mudlib，所以紀錄放在這裡。
#define DEVOUR_LOG "/data/daemon/yaksa_devour.log"

private void devour_log(string text) {
    assure_file(DEVOUR_LOG);
    write_file(DEVOUR_LOG, text);
}

// 門檻（使用者指示，2026-11-04）：鬼魂的精（開始吞時的目前值）與夜叉自己的精
// （最大值）都要達到門檻，才加精氣神、才計數。第 1～10 隻 500，第 11 隻起每隻多 50（550、600……）。
// 夜叉族的鬼魂門檻減半：250，第 11 隻起每隻多 25（275、300……）。
int devour_threshold(int count, int yaksa_ghost) {
    int extra = count >= 10 ? count - 9 : 0;
    return yaksa_ghost ? 250 + 25 * extra : 500 + 50 * extra;
}

private string who(object ob) {
    return sprintf("%s(%s)", ob->name(1), ob->query("id"));
}

// 吞完玩家鬼魂：達到門檻才依原始公式給予永久加成並計數；沒有反噬。
private void reward_player(object me, object victim, int ghost_gin) {
    int count, big, gin, kee, sen, yaksa_ghost, need, raw_gin, own_gin;
    string note;

    raw_gin = ghost_gin;
    // 夜叉自己的精（最大值）也要達到同一個門檻（使用者指示，2026-11-04）。
    own_gin = me->query_stat_maximum("gin");

    count = me->query(COUNT_KEY);
    yaksa_ghost = victim->query_race() == "yaksa";
    need = devour_threshold(count, yaksa_ghost);

    if (ghost_gin < need || own_gin < need) {
        tell_object(me, HIK + (ghost_gin < need
            ? "這縷魂魄太過稀薄，沒能為你增添半分力量。\n"
            : "你的根基尚淺，化不開這縷魂魄的力量，沒能增添半分。\n") + NOR);
        note = ghost_gin < need ? "鬼魂未達門檻，不計數、不加成"
                                : "夜叉自身未達門檻，不計數、不加成";
    } else {
        me->add(COUNT_KEY, 1);
        // 夜叉族的鬼魂，精以兩倍計算（門檻比的是加倍前的精）。
        if (yaksa_ghost) ghost_gin *= 2;
        // 前 10 次必定大幅增加；之後機率 = 10 / (10 + random(吞食次數))。
        big = count < 10 || random(10 + rnd(count)) < 10;
        if (big) {
            gin = 1 + ghost_gin / 40;
            kee = 1 + rnd(ghost_gin / 40);
            sen = 1 + rnd(ghost_gin / 80);
        } else {
            gin = 1 + random(2);
            kee = random(2);
            sen = 0;
        }
        grow(me, "gin", gin);
        grow(me, "kee", kee);
        grow(me, "sen", sen);
        tell_object(me, HIG "你的精增加了 " + gin + " 點"
            + (kee ? "、氣增加了 " + kee + " 點" : "")
            + (sen ? "、神增加了 " + sen + " 點" : "") + "！\n" NOR);
        note = sprintf("計入第 %d 隻，精 +%d、氣 +%d、神 +%d", count + 1, gin, kee, sen);
    }

    devour_log(sprintf("[%s] 夜叉 %s 吞噬 %s（%s）：鬼魂精 %d／夜叉精 %d／門檻 %d，%s\n",
        ctime(time()), who(me), who(victim), victim->query_race(),
        raw_gin, own_gin, need, note));
}

private void finish(object me, object victim, int total) {
    int ghost_gin;

    message_vision(HIR "$N張口一吸，將$n最後一縷魂魄吞進肚裡！\n" NOR, me, victim);
    ghost_gin = me->query_temp("yaksa_devour/ghost_gin");

    if (userp(victim)) {
        reward_player(me, victim, ghost_gin);
        stop_devour(me);
        // 不在這裡直接 die() + make_mist()：魂飛魄散後的選種族畫面與輸入，
        // 會綁在「目前下指令的人」身上，在這裡就是夜叉，結果變成夜叉看到選種族、
        // 被吞的玩家什麼都沒看到。改為耗盡對方的形體（HP），讓對方在自己的心跳裡
        // 走鬼魂再死一次的正常流程（die() → make_mist()），畫面才會出現在對方身上。
        victim->consume_stat("HP", victim->query_stat("HP") + 1, me);
        return;
    }
    stop_devour(me);

    // 鬼魂類 NPC：吞多少補多少精、神（目前值與受損的最大值）。
    me->heal_stat("gin", total);
    me->supplement_stat("gin", total);
    me->heal_stat("sen", total);
    me->supplement_stat("sen", total);
    tell_object(me, HIG "吞下的鬼氣補回了你的精神。\n" NOR);
    destruct(victim);
}

void devour_tick(object me, object victim, int serial) {
    int bite, left, total;

    if (!objectp(me) || me->query_temp("yaksa_devour/serial") != serial) return;
    if (!objectp(victim) || environment(victim) != environment(me)
    ||  victim->query("life_form") != "ghost"
    ||  !living(me) || me->is_fighting() || me->query("life_form") == "ghost") {
        stop_devour(me);
        tell_object(me, "你停止了吞食。\n");
        return;
    }

    // 每口吞食量 = 膽識 + 機敏 + random(自己的精)/10，從鬼魂的精扣除。
    bite = me->query_attr("cor") + me->query_attr("dex") + rnd(me->query_stat("gin")) / 10;
    left = victim->query_stat("gin");
    if (bite > left) bite = left;
    total = me->query_temp("yaksa_devour/total") + bite;
    me->set_temp("yaksa_devour/total", total);

    if (left - bite <= 0) {
        victim->set_stat_current("gin", 0);
        finish(me, victim, total);
        return;
    }
    victim->set_stat_current("gin", left - bite);
    message_vision(HIM "$N大口吞食著$n的魂魄，$n的身形越來越淡了....\n" NOR, me, victim);

    // 吞食期間夜叉無法行動。鬼魂 NPC 也無法行動；玩家的鬼魂可以走開，
    // 離開房間就中斷吞食。
    me->start_busy(1);
    if (!userp(victim)) victim->start_busy(1);
    call_out("devour_tick", TICK_SECONDS, me, victim, serial);
}

int main(object me, string arg) {
    object victim;
    int serial;

    if (me->query_race() != "yaksa") return notify_fail("你不是夜叉，無法吞食鬼魂。\n");
    if (DEVOUR_SEALED)
        return notify_fail("你體內的噬魂凶性已被封住，再也吞不下任何魂魄。\n");
    if (me->query("life_form") == "ghost") return notify_fail("你自己已經是鬼魂了。\n");
    if (me->query_temp("yaksa_devour")) return notify_fail("你正在吞食鬼魂。\n");
    if (me->is_busy()) return notify_fail("你正忙著。\n");
    if (me->is_fighting()) return notify_fail("戰鬥中無法吞食鬼魂。\n");
    if (!arg || !objectp(victim = present(arg, environment(me)))
    ||  !victim->is_character() || victim == me || !victim->visible(me))
        return notify_fail("你要吞食誰？\n");
    if (victim->query("life_form") != "ghost") return notify_fail("你只能吞食鬼魂。\n");
    if (wizardp(victim)) return notify_fail("你無法吞食巫師。\n");
    if (environment(me)->query("no_fight")) return notify_fail("這裡不能吞食鬼魂。\n");

    serial = time() + random(100000);
    me->set_temp("yaksa_devour", ([ "serial": serial, "total": 0,
        "ghost_gin": victim->query_stat("gin") ]));
    message_vision(HIR "$N張開血盆大口，一把抓住$n，開始吞食$n的魂魄！\n" NOR, me, victim);
    tell_object(victim, HIR "你被夜叉抓住了，快逃離這裡！\n" NOR);
    // 下指令後先等 2 tick 才吞第一口（使用者指示，2026-11-04），讓鬼魂有時間逃走。
    me->start_busy(2);
    call_out("devour_tick", TICK_SECONDS * 2, me, victim, serial);
    return 1;
}

int help(object me) {
    // 完整規則見 docs/CUSTOM_RACE_A_H.md（@HELP 區塊不可超過 1024 位元組）。
    write(@HELP
指令格式：devour <鬼魂>

夜叉可以吞食鬼魂 NPC 或死去玩家的鬼魂：
- 抓住鬼魂後要稍待片刻才開始吞食，鬼魂可以趁機逃走。
- 吞鬼魂 NPC：鬼魂被吃掉，吞多少補多少精、神。
- 吞玩家鬼魂：對方魂飛魄散，須重新創造角色。魂魄夠凝實、自身
  根基也夠深厚，才能永久增加精氣神；吞得越多，要求越高。
HELP);
    return 1;
}
