

#pragma save_binary

//#include <net/dns.h>
#include <origin.h>
#include <ansi.h>

inherit F_CLEAN_UP;

private object acquire_login_ob(string id);

private void create() { seteuid(getuid()); }

string
age_string(int time)
{
    int month, day, hour;

    time /= 3600;
    hour = time % 24;
    time /= 24;
    day = time % 30;
    month = time / 30;
    return (month ? month + "m" : "") + (day ? day + "d" : "") + hour + "h";
}

// 總表用的時數：有月份時天、小時固定兩位數（例如 4m05d18h），否則 3d05h（#27，2026-11-03）。
private string table_age(int time)
{
    int month, day, hour;

    time /= 3600;
    hour = time % 24;
    time /= 24;
    day = time % 30;
    month = time / 30;
    if( month ) return sprintf("%dm%02dd%02dh", month, day, hour);
    return sprintf("%dd%02dh", day, hour);
}

// 連線位址：數字位址每段補成三格，主機名稱照原樣（#27，2026-11-03）。
private string table_ip(string ip)
{
    string *part;
    int i;

    if( !stringp(ip) ) return "";
    part = explode(ip, ".");
    if( sizeof(part) != 4 ) return ip;
    for(i=0; i<4; i++) {
        if( !sizeof(part[i]) || sizeof(part[i]) > 3 ) return ip;
        foreach( int ch in part[i] ) if( ch < '0' || ch > '9' ) return ip;
        part[i] = sprintf("%3s", part[i]);
    }
    return implode(part, ".");
}

// 巫師用的線上總表（#27，2026-11-03）：NAME 12 個中文字寬、ID 12 格靠左；DNS、AGE、LEVEL 靠右。
string
finger_all()
{
    object *ob;
    string msg;
    int i;

    ob = sort_array( users(), (: strcmp(query_ip_number($1), query_ip_number($2)) :) );
    msg = cjk_pad("NAME", 24) + " " + cjk_pad("ID", 12) + " " + cjk_pad("DNS", 15, 1)
        + "  " + cjk_pad("AGE", 9, 1) + " " + cjk_pad("LEVEL", 5, 1) + "\n";
    for(i=0; i<sizeof(ob); i++) {
	if( this_player() && !ob[i]->visible(this_player()) ) continue;
	msg += cjk_pad(ob[i]->query("name") || "", 24) + " "
	    + cjk_pad(ob[i]->query("id") || "", 12) + " "
	    + cjk_pad(table_ip(query_ip_name(ob[i])), 15, 1) + "  "
	    + cjk_pad(ob[i]->link() ? table_age(ob[i]->link()->query("time_aged")) : "n/a", 9, 1) + " "
	    + cjk_pad(sprintf("%d", ob[i]->query("level")), 5, 1) + "\n";
    }
    return msg;
}

string
finger_user(string name)
{
    object ob, body;
    string msg, mud;
    int invis, age, wiz_mode = 0;

#if 0
    if( sscanf(name, "%s@%s", name, mud)==2 ) {
	GFINGER_Q->send_gfinger_q(mud, name, this_player(1));
	return "網路指令傳送過程可能需要一些時間﹐請稍候。\n";
    }
#endif

    ob = acquire_login_ob(name);
    if( !ob ) return "沒有這位使用者。\n";
    body = find_player(name);

    if( this_player(1)
    && (wizardp(this_player(1)) || body==this_player(1)) ) wiz_mode = 1;

    age = ob->query("time_aged");
    invis = ob->query("invis") && (!body || !this_player()
	|| wiz_level(body) > wiz_level(this_player()));

    // higher level wiz can use invis.c to be invisible to lowers
    msg =  sprintf("英文代號：%-16s姓    名：%s\n權限等級：%-16s"
	"電子郵件：%s\n",
	capitalize(ob->query("id")),
	ob->query("name"),
	wizhood(name),
	wiz_mode ? ob->query("email") : "<限巫師查詢>" );

    if( body && !invis && objectp(body) ) {
	if( interactive(body) ) {
	    msg += sprintf("\n%s正從 %s 連線中，連線時數%s。\n",
		gender_pronoun(body),
		wiz_mode ? query_ip_name(body) : "<某處>",
		chinese_period(time() - ob->query_temp("login_time")) );
	    msg += sprintf("角色狀態：等級 %d %s\n",
		body->query_level(), body->rank() );
	}
	else msg += "\n" + gender_pronoun(body) + "現在斷線中。\n";
    }

    // 上次連線的位址只有巫師（或查自己）看得到（#27，2026-11-03）。
    msg += sprintf("\n上次連線：%s (%s)\n",
	wiz_mode ? ob->query("last_from") : "<限巫師查詢>", ctime(ob->query("last_on")));

    if( invis ) age -= time() - ob->query_temp("login_time");
    msg += sprintf("時數總和：%s\n", chinese_period(age));

    if( !ob->body() ) destruct(ob);

    return msg;
}

varargs string
remote_finger_user(string name, int chinese_flag)
{
    object ob, body;
    string msg;

    ob = acquire_login_ob(name);
    if( !ob ) return chinese_flag ? "沒有這個玩家。\n" : "No such user.\n";
    if( chinese_flag ) msg =  sprintf(
	"\n英文代號﹕\t%s\n姓    名﹕\t%s\n權限等級﹕\t%s\n"
	"電子郵件地址﹕\t%s\n上次連線地址﹕\t%s( %s )\n\n",
	ob->query("id"),
	ob->query("name"),
	SECURITY_D->get_status(name),
	ob->query("email"),
	ob->query("last_from"),
	ctime(ob->query("last_on"))
    );
    else msg =  sprintf(
	"\nName\t: %s\nStatus\t: %s\nEmail\t: %s\nLastOn\t: %s( %s )\n\n",
	capitalize(ob->query("id")),
	SECURITY_D->get_status(name),
	ob->query("email"),
	ob->query("last_from"),
	ctime(ob->query("last_on"))
    );
    if( body = find_player(name) ) {
	if( !this_player() || this_player()->visible(body) )
	    msg += chinese_flag ?
		("\n" + ob->query("name") + "目前正在線上。\n"):
		("\n" + capitalize(name) + " is currently connected.\n");
    }

    if( !ob->body() ) destruct(ob);
    return msg;
}

object
acquire_login_ob(string id)
{
    object ob;

    if( origin()==ORIGIN_CALL_OTHER ) {
	if( geteuid(previous_object())!=ROOT_UID
	&&  base_name(previous_object())!=MAILBOX_OB ) return 0;
    }

    if( ob = find_player(id) ) {
	// Check if the player is linkdead
	if( ob->link() ) return ob->link();
    }

    ob = new(LOGIN_OB);
    // 暫時物件的身分沒有讀檔權限，先交付本程式的權限，才讀得到離線玩家的存檔（#27，2026-11-03）。
    export_uid(ob);
    ob->set("id", id);
    return ob->restore() ? ob : 0;
}

