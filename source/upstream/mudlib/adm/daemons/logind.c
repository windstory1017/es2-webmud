/*---
description: login daemon, used to handle user login and character creation.
author: Annihilator <taedlar@gmail.com>
---*/
#pragma save_binary

#include <ansi.h>
#include <command.h>
#include <login.h>
#include <localtime.h>

inherit F_CLEAN_UP;
inherit F_DBASE;

int wiz_lock_level = WIZ_LOCK_LEVEL;

#define IPLIMIT_MSG "同一處來的俠客已然滿座，請稍候再來。\n"

string *user_race = ({
    "human",
    "avatar",
    "blackteeth",
    "yenhold",
    "jiaojao",
    "woochan",
    "dingling",
    "headless",
    "rainner",
    "malik",
    "yaksa",
    "ashura",
});

string *banned_name = ({
    "你", "妳", "我", "他", "她", "牠", "它",
    "幹", "操", "娘", "媽", "屌", "屄", "插", "姦", "穴",
});

string *banned_ip = ({
});

string *banned_hostname = ({
});

#ifdef ENABLE_ANTISPAM
// 同一個 IP 從建立第一個角色起算 30 分鐘內，最多建立 10 個角色。
// 不再扣新角色的屬性。ip : ({ 第一個角色的建立時間, 建立數量 })
#define SPAM_WINDOW	1800
#define SPAM_LIMIT	10
mapping spammer_ip = ([]);

private int create_limited(string ip) {
    mixed *rec = spammer_ip[ip];
    return arrayp(rec) && time() - rec[0] < SPAM_WINDOW && rec[1] >= SPAM_LIMIT;
}

private void count_creation(string ip) {
    mixed *rec = spammer_ip[ip];
    if (!arrayp(rec) || time() - rec[0] >= SPAM_WINDOW)
        spammer_ip[ip] = ({ time(), 1 });
    else
        rec[1]++;
}
#endif

private void get_id(string arg, object ob);
private void confirm_id(string yn, mapping args, object ob);
private void get_email(string email, object ob);
private void show_race_menu(object ob);
private void authorize(object ob);
object make_body(object ob);
private void init_new_body(object link, object user);
varargs void enter_world(object ob, object user, int silent);
varargs void reconnect(object ob, object user, int silent);
object find_body(string name);
int check_legal_id(string arg);
int check_legal_name(string arg);
int check_new_id(string arg);
private void increment_visitor_count();
private string login_date_string(int t);
private void show_visitor_count();
private int check_ip(object link);

private void create() {
    seteuid (getuid());
    set ("channel_id", "連線精靈");
}

private void reset() {
    log_file ("USRGRAPH", sprintf ("[%s] %d users\n", ctime(time()), sizeof(users())));
#ifdef ENABLE_ANTISPAM
    spammer_ip = filter(spammer_ip, (: time() - $2[0] < SPAM_WINDOW :));
#endif
}

void logon (object ob) {
    object *usr;
    int i, wiz_cnt, ppl_cnt, login_cnt;

    if (ob.getuid() != ROOT_UID)
        error ("Insecure user object."); // only allow new user object created with ROOT_UID.

    seteuid (getuid());
    write (read_file (WELCOME) + "\n");

    printf ("現在時間 %s, 東方故事Ⅱ已經執行了%s。\n",
        login_date_string(time()), CHINESE_D->chinese_period(uptime()));
    show_visitor_count();

    usr = users();
    wiz_cnt = 0;
    ppl_cnt = 0;
    login_cnt = 0;
    // invis wizard count in ppl in stead of wiz, by grain (03/25/1998)
    for (i=0; i<sizeof(usr); i++) {
        if (!usr[i].environment())
            login_cnt++;
        else if (wizardp(usr[i])) {
            if (!usr[i]->link()->query("invis"))
                wiz_cnt++;
        }
        else
            ppl_cnt++;
    }
    printf ("目前共有 %d 位巫師、%d 位玩家在線上﹐以及 %d 位使用者嘗試連線中。\n\n",
        wiz_cnt, ppl_cnt, login_cnt );

    write ("如果您是第一次使用，請輸入您喜歡的使用者代號以註冊角色\n");
    write ("您的使用者代號：");
    input_to ("get_id", ob);
}

// 登入畫面用的日期格式：2026-10-04 13:17:49
private string login_date_string(int t) {
    mixed *lt = localtime(t);

    return sprintf("%d-%02d-%02d %02d:%02d:%02d", lt[LT_YEAR], lt[LT_MON] + 1,
        lt[LT_MDAY], lt[LT_HOUR], lt[LT_MIN], lt[LT_SEC]);
}

private void show_visitor_count() {
    string s = read_file ("/adm/etc/visitor.cnt");
    int t, cnt;

    if( !s || sscanf(s, "%d %d", t, cnt) != 2 ) return;
    printf ("從 %s 以來累計上線人次：%d 人次。\n", login_date_string(t), cnt);
}

// realipd 出錯時一律放行，不能因此擋住所有人登入。
private int ip_can_enter(object ob, string id, object except) {
    int ok;

    if (catch (ok = REALIP_D->can_enter(ob, id, except)))
        return 1;
    return ok;
}

private void get_id (string arg, object ob) {
    object ppl;

    // 網頁橋接程式在連線一開始送來的真實 IP（見 /adm/daemons/realipd.c），不顯示任何東西。
    if (arg && arg[0..8] == "@@REALIP ") {
        catch (REALIP_D->register_line(ob, arg));
        input_to ("get_id", ob);
        return;
    }

    // user id will be used as object uid for all objects created by the user.
    // the id string must be converted too all lower case, while objects created by backend are assigned
    // an uid with first character captialized. Thus we prevents a user called himself 'root' to become
    // privileged.
    arg = arg.lower_case ();

    if (!check_legal_id(arg)) {
        write ("您的使用者代號: ");
        input_to ("get_id", ob);
        return;
    }

    if (ob.getuid() != ROOT_UID) {
        write ("權限設定失敗。\n");
        return;
    }
    ob->set("id", arg);

#ifdef MAX_USERS
    if (wizhood(arg) == "(player)" && sizeof(users()) >= MAX_USERS) {
        ppl = find_body (arg);
        // Only allow reconnect an interactive player when MAX_USERS exceeded.
        if (!ppl || !ppl.interactive()) {
            write ("目前連線中的使用者已達上限，請稍後重新嘗試連線。\n");
            destruct (ob);
            return;
        }
    }
#endif

#ifdef ENABLE_BAN_SITE
    // Rework by Annihilator (11/10/1999), support IP address and hostname
    if (wizhood(arg)=="(player)") {
        string ip, pattern;

        ip = query_ip_number(ob);
        foreach(pattern in banned_ip)
            if( ip==pattern || sscanf(ip, pattern) ) {
                write("您的連線位置目前不接受使用者登入。\n");
                destruct (ob);
                return;
            }
        ip = query_ip_name(ob);
        foreach(pattern in banned_hostname)
            if( ip==pattern || sscanf(ip, pattern) ) {
                write("您的連線位置目前不接受使用者登入。\n");
                destruct (ob);
                return;
            }
    }
#endif

    // 同一處來的角色已達上限（重新連線回原本還在世界裡的角色不受影響）。
    if (!find_body(arg) && !ip_can_enter(ob, arg, 0)) {
        write (IPLIMIT_MSG);
        destruct (ob);
        return;
    }

#ifdef WIZ_LOCK_LEVEL
    if ((int)wiz_level(arg) < (int)wiz_lock_level) {
        write(MUD_NAME + "目前限制巫師等級 " + WIZ_LOCK_LEVEL
            + " 以上的人才能連線。\n");
        destruct (ob);
        return;
    }
#endif

    if (arg == "guest") {
        // let guest create a character.
        seteuid (arg);
        export_uid (ob);
        seteuid (getuid());
        get_email ("none", ob);
        return;
    }
    else if (file_size (login_data (arg)) != -1) {
        seteuid (arg);
        export_uid (ob);
        seteuid (getuid());

        if (ob->restore()) {
            if (ob.userp() == 2) { // console user
                write ("\n");
                authorize (ob);
                return;
            }
            write ("請輸入密碼: ");
            input_to ("get_passwd", 1, ob);
            return;
        }
        write ("對不起﹐您的人物儲存檔出了一些問題﹐請利用 guest 人物通知巫師處理。\n");
        destruct (ob);
        return;
    }

    // 新帳號只能用 a 到 z 的英文字母（使用者指示，2026-10-19）；
    // 舊帳號照 check_legal_id() 原規則，含數字的舊帳號仍可登入。
    if (!check_new_id(arg)) {
        write ("您的使用者代號: ");
        input_to ("get_id", ob);
        return;
    }

    mapping opts = ([
        "prompt": "使用 " + arg + " 這個代號將會創造一個新的人物﹐您確定嗎？",
        "options": ({ "Y) 是", "N) 否" }),
        "cursor": 1
    ]);
    input_to ("confirm_id", opts, ob);
}

private void get_passwd(string pass, object ob) {
    string my_pass;

    write("\n");

    if( !check_ip(ob) ) {
        write("對不起，您的連線位置不正確。\n");
        destruct (ob);
        return;
    }

    my_pass = ob->query("password");
    if( crypt(pass, my_pass) != my_pass ) {
        write("密碼錯誤！\n");
        destruct (ob);
        return;
    }
    authorize(ob);
}

void authorize (object ob) {
    object user = find_body (ob->query("id"));
    if (user) {
        if (!user->link()) {
            reconnect (ob, user);
            return;
        }
        mapping opts = ([
            "prompt": "您要將另一個連線中的相同人物趕出去﹐取而代之嗎? ",
            "options": ({ "Y) 是", "N) 否" }),
            "cursor": 1
        ]);
        input_to ("confirm_relogin", opts, ob, user);
        return;
    }

    user = make_body(ob);
    if (! user) {
        destruct (ob);
        return;
    }

    if (user->restore()) {
        log_file ("USAGE", sprintf("[%s] %s login from %s\n",
            ctime(time()), (string)user->query("id"), query_ip_name(ob) ) );

        if (wizhood(ob) == "(admin)") {
            // 用連線本身的位址：admin 照舊只能從主機本機（含網頁橋接程式）登入。
            if ((efun::query_ip_name(ob) != "localhost") && (efun::query_ip_number(ob) != "127.0.0.1")) {
                write ("安全檢查失敗！強制登出。\n");
                destruct (user);
                destruct (ob);
                return;
            }
            write ("安全檢查通過。\n");
        }
        enter_world (ob, user);
        return;
    } else {
        if (file_size(user->query_save_file()) == -1 && ob->query("reincarnating")) {
            // 魂飛魄散後還沒轉世就斷線：直接回到選種族（使用者定案，2026-10-22）。
            destruct (user);
            show_race_menu (ob);
            return;
        }
        if (file_size(user->query_save_file()) == -1) {
            write (cjk_wrap (@NOTICE
系統找不到您的角色資料。
可能的原因包括您在創造角色時斷線，或者因為其他原因導致角色資料被刪除。
如果您認為並沒有上述這些情況，請中斷連線並且用 guest 帳號洽線上巫師確認。
NOTICE
            , 70) + "\n\n");
            destruct (user);
            mapping opts = ([
                "prompt": "您要重新創造這個角色嗎? ",
                "options": ({ "Y) 是", "N) 否" }),
                "cursor": 1
            ]);
            input_to ("confirm_reincarnate", opts, ob);
        } else {
            write (cjk_wrap (@NOTICE
系統目前無法讀取您的人物資料，可能的原因包括系統正在備分或整理使用者資料，請稍候再試。
NOTICE
            , 70));
            destruct (user);
            destruct (ob);
        }
    }
}

private void confirm_reincarnate (string yn, mapping opts, object ob) {
    string answer = cursor_translate (yn, opts);
    switch (answer ? answer : yn) {
        case "Y": case "y":
            break;
        case "N": case "n":
            write ("好吧﹐歡迎下次再來。\n");
            destruct (ob);
            return;
        default:
            input_to ("confirm_reincarnate", opts, ob);
            return;
    }
    write ("\r" CLR "\n");
    show_race_menu (ob);
}

private void confirm_relogin (string yn, mapping opts, object ob, object user) {
    string answer = cursor_translate (yn, opts);
    object old_link;

    switch (answer ? answer : yn) {
        case "Y": case "y":
            break;
        case "N": case "n":
            write ("好吧﹐歡迎下次再來。\n");
            destruct (ob);
            return;
        default:
            input_to ("confirm_relogin", opts, ob, user);
            return;
    }
    tell_object (user, "有人從別處( " + query_ip_number (ob) + " )連線取代您所控制的人物。\n");
    log_file ("USAGE", sprintf ("[%s] %12s replaced @ %s\n",
        ctime (time()),
        (string)user->query("id"),
        query_ip_name (ob))
    );

    // Kick out the old player.
    old_link = user->link();
    if (old_link) {
        seteuid (getuid());
        if (user.interactive())
            exec (old_link, user);
        destruct (old_link);
    }

    reconnect (ob, user);
}

/* Asked the user to confirm a non-existing username */
private void confirm_id (string yn, mapping opts, object ob) {
    string answer = cursor_translate (yn, opts);
    switch (answer ? answer : yn) {
        case "Y": case "y":
            break;
        case "N": case "n":
            write ("\r" CLR "請重新輸入您的使用者代號: ");
            input_to ("get_id", ob);
            return;
        default:
            input_to ("confirm_id", opts, ob);
            return;
    }

#ifdef ENABLE_ANTISPAM
    if (create_limited(query_ip_number(ob))) {
        write("\r" CLR "從您連線的主機創造的人物太多了﹐請過一段時間再來創造新人物。\n");
        destruct (ob);
        return;
    }
#endif
    seteuid (ob->query("id"));
    export_uid (ob);
    seteuid (getuid());

    write ("\r" CLR "請設定您的密碼: ");
    input_to ("new_password", 1, ob);
}

private void new_password(string pass, object ob) {
    write ("\n");
    if (strlen(pass) < 5) {
        write ("密碼的長度至少要五個字元﹐請重設您的密碼: ");
        input_to ("new_password", 1, ob);
        return;
    }
#ifdef	ENABLE_MD5_PASSWORD
    ob->set ("password", crypt (pass, sprintf ("$1$%d", random(99999999))));
#else
    ob->set ("password", crypt (pass, 0) );
#endif
    write ("請再輸入一次您的密碼﹐以確認您沒記錯: ");
    input_to ("retype_password", 1, ob);
}

private void retype_password (string pass, object ob) {
    string old_pass;

    write ("\n");
    old_pass = ob->query("password");
    if (crypt (pass, old_pass) != old_pass) {
        write ("您兩次輸入的密碼並不一樣﹐請重新設定一次密碼: ");
        input_to ("new_password", 1, ob);
        return;
    }

    write (cjk_wrap (@TEXT
為了避免您的人物遭人盜用﹐ES2 採用 email 認證方式保護人物所有權。
這個電子郵件地址除了巫師以外﹐不會被其他使用者看到。
如果您同意的話，請提供一個可供接收認證用電子郵件的地址。
TEXT
    , 70));
    write ("\n\n您的電子郵件地址 (或 none): ");
    input_to ("get_email", ob);
}

private void get_email (string email, object ob) {
    int delim = 0, err = 0;

    if (email != "none") {
        if (email.len() > 64) {
            write ("電子郵件地址最多可以有 64 個字元。\n");
            write ("您的電子郵件地址 (或 none): ");
            input_to ("get_email", ob);
            return;
        }

        foreach (int ch in email) {
            if (ch=='@' && !delim) {
                delim = 1;
                continue;
            }
            if (strsrch ("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.", ch) < 0) {
                err = 1;
                break;
            }
        }
        if (!delim || err) {
            write ("您的電子郵件格式錯誤，請輸入正確的電子郵件地址。\n");
            write ("您的電子郵件地址 (或 none): ");
            input_to ("get_email",  ob);
            return;
        }
    }

    ob->set ("email", email);

//  If ONE_GUEST is defined in /include/login.h, only permit one guest login.
#ifdef ONE_GUEST
    if ((string)ob->query("id") == "guest" && find_player ("guest")) {
        write ("目前線上已經有一位訪客了，請稍後再試。\n");
        destruct(ob);
        return;
    }
#endif

    // Complete non-body-specific initialization of new user here.
    // 新帳號有 20 點業力（使用者定案，2026-10-22）。
    ob->set ("karma", 20);
    show_race_menu (ob);
}

/* 業力系統（使用者定案，2026-10-22）：
 * 選種族時扣除該種族的業力；只列出業力足夠的種族，依業力由低到高、同業力依英文名排列。
 * 人類族不能直接選，選人類時有 1% 機會自動成為人類族。
 * 連最便宜的種族都選不起時，帳號消失。巫師不受業力限制。 */
private int is_wizard_account (object ob) {
    return wizhood(ob->query("id")) != "(player)";
}

private int race_karma (string race) {
    return (int)RACE_D(race)->query("karma");
}

private int sort_race (string a, string b) {
    int ka = race_karma(a), kb = race_karma(b);
    if (ka != kb) return ka - kb;
    return strcmp(a, b);
}

/* 顯示寬度：中文字算兩格。 */
private int text_width (string str) {
    int w = 0;
    foreach (int ch in str) w += (ch >= 0x2E80) ? 2 : 1;
    return w;
}

private string *affordable_races (object ob) {
    int karma = ob->query("karma");
    string *list = filter(user_race, (: $1 != "avatar" :));
    if (!is_wizard_account(ob))
        list = filter(list, (: race_karma($1) <= $2 :), karma);
    return sort_array(list, (: sort_race :));
}

private void delete_account (object ob) {
    string id = ob->query("id");

    write ("\n你的業力已盡，再也無法轉世投胎。這段江湖路，就此走到了盡頭。\n");
    log_file ("USAGE", sprintf("[%s] %s karma exhausted, account removed\n", ctime(time()), id));
    if (stringp(id)) {
        rm (login_data(id));
        rm (user_data(id));
    }
    destruct (ob);
}

private void show_race_menu (object ob) {
    string *list = affordable_races(ob), line;

    if (!sizeof(list)) {
        delete_account (ob);
        return;
    }
    // 魂飛魄散後由這裡重新選種族：這時的 input_to 不會經過 input_prompt()，
    // 直接通知網頁暫停背景狀態查詢（恢復見 feature/char/message.c 的 write_prompt()）。
    if (ob->query_temp("webhud/client")) {
        ob->set_temp("webhud/input_wait", 1);
        write ("\n@@WEBHUD|INPUT\n");
    }
    write (sprintf("您現在共有 %d 點業力，可以選擇以下的種族：\n", (int)ob->query("karma")));
    foreach (string race in list) {
        line = to_chinese(race) + " (" + race + ")";
        write (line + repeat_string(" ", 26 - text_width(line)) + race_karma(race) + " 點業力\n");
    }
    write ("您的選擇(用 '? <種族名>' 可查閱說明)﹕");
    input_to ("get_race", ob, list);
}

/* 依英文名（不分大小寫）或中文名找種族。 */
private string match_race (string arg, string *list) {
    foreach (string r in list)
        if (lower_case(arg) == r || arg == to_chinese(r)) return r;
    return 0;
}

private void get_race (string arg, object ob, string *list) {
    string race;

    if (!stringp(arg)) arg = "";
    arg = replace_string(arg, " ", "");
    // 網頁背景的狀態查詢（webhud）不是玩家的答案，不要回「請重新輸入」。
    if (arg == "" || arg == "webhud") {
        write ("您的選擇(用 '? <種族名>' 可查閱說明)﹕");
        input_to ("get_race", ob, list);
        return;
    }
    if (arg[0] == '?') {
        // 說明可以查任何可選的種族（人類族除外）。
        race = match_race(arg[1..], filter(user_race, (: $1 != "avatar" :)));
        if (race) {
            // 和遊戲裡的 help <種族> 讀同一份檔案（使用者定案，2026-10-22）。
            string doc = read_file(HELP_DIR + race + ".txt");
            if (stringp(doc)) write ("\n" + doc + "\n");
            else {
                CHAR_D->hint_user_race(race);
                write ("\n");
            }
        } else
            write ("請重新輸入\n");
        write ("您的選擇(用 '? <種族名>' 可查閱說明)﹕");
        input_to ("get_race", ob, list);
        return;
    }
    race = match_race(arg, list);
    if (!race) {
        // 種族存在但不在清單上＝業力不足；其他字串＝輸入錯誤（使用者定案，2026-10-22）。
        if (match_race(arg, filter(user_race, (: $1 != "avatar" :))))
            write ("你的業力不足。\n");
        else
            write ("請重新輸入\n");
        write ("您的選擇(用 '? <種族名>' 可查閱說明)﹕");
        input_to ("get_race", ob, list);
        return;
    }

    if (!is_wizard_account(ob))
        ob->add ("karma", -race_karma(race));
    if (race == "human" && random(100) < 1)
        race = "avatar";
    write (sprintf("你花費了 %d 點業力﹐投胎成為%s。\n", race_karma(race == "avatar" ? "human" : race), to_chinese(race)));

    mapping gender_opts = ([
        "prompt": "您要扮演的哪種性別的角色? ",
        "options": ({ "M) 男性", "F) 女性" }),
        "cursor": 0
    ]);
    input_to ("get_gender", gender_opts, ob, race);
}

private void get_gender (string gender, mapping opts, object ob, string race) {
    object body;
    string body_file;

    string answer = cursor_translate (gender, opts);
    switch (answer ? answer : gender) {
        case "M": case "m":
            gender = "male";
            break;
        case "F": case "f":
            gender = "female";
            break;
        default:
            input_to ("get_gender", opts, ob, race);
            return;
    }

    if (!stringp (body_file = RACE_D(race)->query("default_body")))
        body_file = USER_OB;
    ob->set ("body", body_file);
    if( !(body = make_body (ob)) ) {
        destruct (ob);
        return;
    }

    // Remember it so we can dest it if we go linkdead before finishing
    // chraracter creation.
    ob->set_temp("temp_body", body);
    body->set ("gender", gender); // Only for appearance
    body->set_race (race);
    init_new_body (ob, body);

    write ("\r" CLR "您的顯示名稱: ");
    input_to ("get_name", ob, body);
}

private void get_name (string arg, object ob, object user) {
    if (arg == "webhud" || !check_legal_name(arg)) {
        write ("您的顯示名稱: ");
        input_to ("get_name", ob, user);
        return;
    }

    ob->set("name", arg);
    user->set("name", arg);
    if (!ob->query ("creation_time")) {
        ob->set ("creation_time", time());
        // default open chat and rumor channel -- by dragoon
        ob->set ("channels", ({ "chat", "rumor" }));
    }

    mapping opts = ([
        "prompt": "您確定要創造這個角色嗎? ",
        "options": ({ "Y) 是", "N) 否" }),
        "cursor": 0
    ]);
    input_to ("confirm_incarnate", opts, ob, user);
}

private void confirm_incarnate (string yn, mapping opts, object ob, object user) {
    string answer = cursor_translate (yn, opts);
    switch (answer ? answer : yn) {
        case "Y": case "y":
            break;
        case "N": case "n":
            write ("\r" CLR "好的，歡迎下次再來。");
            destruct (user);
            destruct (ob);
            return;
        default:
            input_to ("confirm_incarnate", opts, ob, user);
            return;
    }
    write ("\r" CLR + user->name() + "在" MUD_NAME "的冒險開始了。\n");
    log_file ("USAGE", sprintf("[%s] %12s created @ %s\n",
        ctime (time()),
        user->query("id"),
        query_ip_name (ob))
    );
    enter_world(ob, user);
}

object make_body (object link_ob) {
    string err;
    object user;
    int n;

    seteuid (getuid());
    err = catch (user = new (USER_OB));
    if (err || !user) {
        write ("現在可能有巫師正在修改使用者物件的程式﹐請稍候再試。\n");
        return 0;
    }

    seteuid (getuid(link_ob));
    export_uid (user);
    seteuid (getuid());

    user->set ("id", link_ob->query("id"));
    user->set_name (link_ob->query("name"), link_ob->query("id"));

    return user;
}

private int check_ip(object link) {                                                                               
    string okip, cur_ip, cur_ip_num, ip_part, num_part, be_checked;
    int len, ed;

    okip = link->query("okip");
    if( !arrayp(okip) || !sizeof(okip) ) return 1;

    cur_ip = query_ip_name(link);
    cur_ip_num = query_ip_number(link);
    foreach(string ip in explode(okip, ":") - ({ "" }))
    {
        if (sscanf(ip, "%s*", be_checked) && be_checked!="")
        {
            len = strlen(be_checked);
            ip_part = cur_ip[0..len-1];
            num_part = cur_ip_num[0..len-1];
        }
        else if (sscanf(ip, "*%s", be_checked) && be_checked!="")       
        {
            len = strlen(be_checked);
            ed = strlen(cur_ip);
            ip_part = cur_ip[ed-len..<1];
            num_part = cur_ip_num[ed-len..<1];
        }
        else
        {
            be_checked = ip;
            ip_part = cur_ip;
            num_part = cur_ip_num;
        }
       
        if (lower_case(be_checked) == lower_case(ip_part)
        || lower_case(be_checked) == lower_case(num_part))
        {
            write(HIW "檢查通過。\n" NOR);
            return 1;
        }
    }

    return 0;
}

private void init_new_body(object link, object user) {
    user->set("birthday", time() );
    user->set_class("commoner");
    user->set_level(1);

#ifdef ENABLE_ANTISPAM
    count_creation(query_ip_number(link));
#endif

    CHAR_D->setup_char(user);
}

varargs void enter_world(object ob, object user, int silent) {
    object room;
    string startroom, err;

    // 登入過程中其他角色可能已經先進入世界，進入前再檢查一次。
    if (!silent && !ip_can_enter(ob, ob->query("id"), user)) {
        write (IPLIMIT_MSG);
        destruct (user);
        destruct (ob);
        return;
    }

    user->set_link(ob);
    ob->set_body(user);

    // finished logon, transfer the interactive user from logon to user body
    exec (user, ob);
    catch (REALIP_D->mark_body(user));

    if (!silent)
        write ("目前權限: " + wizhood(user) + "\n");

    user->setup();
    increment_visitor_count();

    ob->delete ("reincarnating");   // 已經轉世進入遊戲

#ifdef SAVE_USER
    // save newly created user
    user->save();
#endif

    if (silent)
        return;

    cat(MOTD);
    IDENT_D->query_userid((string)user->query("id"));


    startroom = user->query("startroom");
    if( !startroom ) startroom = START_ROOM;
    err = catch(room = load_object(startroom));
    if( !room ) err = catch(room = load_object(VOID_OB));

    if( !room || !user->move(room) ) {
        write(@NOTICE
對不起，目前系統正在整修一些登入地點的程式，請稍候再試。
NOTICE
        );
        destruct(ob);
        destruct(user);
        return;
    }


    if( ob->query("new_mail") ) {
        write( HIW "\n有您的信！請到驛站來一趟 ...\n\n" NOR);
        ob->delete("new_mail");
    }
 
    // if detect mark: pker, set the time mark -dragoon
    if( user->query("pker") ) {
        user->set("last_pk_time", time());
        user->delete("pker");
    }

    if (!wizardp(user) && !user->query("invis")) {
        message ("vision", user->query("name") + "連線進入這個世界。\n", room, user);
        CHANNEL_D->do_channel (this_object(), "sys",
            sprintf ("%s由%s連線進入。",
            user->short(1),
            query_ip_name (user))
        );
    }
}

varargs void reconnect (object ob, object user, int silent) {

    user->set_link(ob);
    ob->set_body(user);
    exec(user, ob);
    catch (REALIP_D->mark_body(user));

    user->reconnect();
    IDENT_D->query_userid((string)user->query("id"));   

    if( silent ) return;

    // if detect pking, reset time mark to now -dragoon
    if( time() - (int)user->query("last_pk_time") < 60 * 60 )
        user->set("last_pk_time", time());


    if( !wizardp(user) && !user->query("invis") ) {
        message("vision", user->query("name") + "重新連線回到這個世界。\n",
                environment(user), user);
        CHANNEL_D->do_channel( this_object(), "sys",
                sprintf("%s(%s)由%s重新連線進入。",
                user->name(1),
                user->query("id"),
                query_ip_name(user))
        );
    }
}

void net_dead(object ob) {
    CHANNEL_D->do_channel( this_object(), "sys",
        sprintf("%s(%s)斷線了。", ob->name(1), ob->query("id")));
    ob->move("/obj/void");
}

int check_legal_id (string id) {
    int i;

    i = strlen (id);
    if ((i < 3) || (i > 12 )) {
        write ("您的使用者代號必須是 3 到 12 個英文字母或數字。\n");
        return 0;
    }
    while (i--)
        if (!(((id[i]>='a') && (id[i]<='z')) ||
              ((id[i]>='0') && (id[i]<='9')))) {
            write("您的使用者代號只能由 a 到 z 的英文字母或 0 到 9 的數字組成。\n");
            return 0;
        }

    return 1;
}

// 新帳號：只能由 a 到 z 的英文字母組成。
int check_new_id (string id) {
    int i = strlen (id);

    while (i--)
        if (id[i] < 'a' || id[i] > 'z') {
            write("新的使用者代號只能使用 a 到 z 的英文字母。\n");
            return 0;
        }
    return 1;
}

// 是不是中文字：UTF-8 三個位元組、落在 CJK 統一漢字（含擴充 A）或相容漢字的範圍。
int is_chinese_char (string ch) {
    int cp;

    if (strlen (ch) != 3) return 0;
    cp = ((ch[0] & 0x0F) << 12) | ((ch[1] & 0x3F) << 6) | (ch[2] & 0x3F);
    return (cp >= 0x4E00 && cp <= 0x9FFF)
        || (cp >= 0x3400 && cp <= 0x4DBF)
        || (cp >= 0xF900 && cp <= 0xFAFF);
}

int check_legal_name(string name) {
    int i;
    string* mbcs = explode(name, ""); // neolith extension, split a UTF-8 string to an array of MBCS characters

    i = sizeof(mbcs);

    // 顯示名稱最少一個字（例如單名「熊」），最多 18 個字。
    if ((i < 1) || (i > 18 )) {
        write("您的顯示名稱必須使用長度 1 到 18 的合法 UTF-8 文字。\n");
        return 0;
    }
    while (i--) {
        if (mbcs[i] <= " ") {
            write ("顯示名稱不能含有控制字元。\n");
            return 0;
        }
        if (mbcs[i] == "　") {
            write ("顯示名稱不可以使用空白字元。\n");
            return 0;
        }
        // 顯示名稱只能用中文（使用者指示，2026-10-19）。
        if (!is_chinese_char (mbcs[i])) {
            write ("顯示名稱只能使用中文字﹐不可以使用英文、數字或符號。\n");
            return 0;
        }
        if (member_array (mbcs[i], banned_name) != -1) {
            write ("顯示名稱含有容易造成其他人的困擾的字元，請換一個名字。\n");
            return 0;
        }
    }
    return 1;
}

object find_body(string name) {
    object ob, *body;

    if( objectp(ob = find_player(name)) ) return ob;
    foreach(ob in children(USER_OB))
        if( clonep(ob)
        &&  userp(ob)
        &&  ob->is_character()
        &&  getuid(ob) == name )
            return ob;

    return 0;
}

int set_wizlock(int level) {
    if( wiz_level(this_player(1)) <= level ) return 0;
    if( geteuid(previous_object()) != ROOT_UID ) return 0;

    wiz_lock_level = level;
    return 1;
}

void reincarnate (object ob) {
    if (previous_object() && geteuid(previous_object()) != ROOT_UID)
        return;

    seteuid (getuid());
    object link = ob->link();
    if (!link) {
#ifdef	SAVE_USER
        ob->save();
#endif
        destruct(ob);
        return;
    }

    /* 魂飛魄散的結算（使用者定案，2026-10-22）：角色業力 = 角色等級，
     * 但不超過帳號上線時數 ÷ 6；加回帳號。 */
    link->update_age();
    int earned = ob->query_level();
    int cap = (int)link->query("time_aged") / 3600 / 6;
    if (earned > cap) earned = cap;
    if (earned > 0) link->add ("karma", earned);
    link->set ("reincarnating", 1);   // 轉世完成（進入遊戲）前斷線，重新登入直接選種族

#ifdef	SAVE_USER
    link->save();
#endif

    exec (link, ob);
#ifdef	SAVE_USER
    rm (ob->query_save_file());
#endif
    destruct(ob);

    show_race_menu (link);
}

#define VISITOR_COUNTER_FILE	"/adm/etc/visitor.cnt"

private void increment_visitor_count() {
    int t, cnt;
    string s = read_file (VISITOR_COUNTER_FILE);

    if (!s) 
        s = sprintf("%d 1", time());
    else {
        sscanf(s, "%d %d", t, cnt);
        if (! t) {
            t = time();
            cnt = 0;
        }
        s = sprintf("%d %d", t, cnt+1);
    }
    write_file (VISITOR_COUNTER_FILE, s, 1);
}
