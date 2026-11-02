/* CUSTOM WEBMUD BOARD BRIDGE
 * 網頁「回報」視窗讀寫「問題回報與建議」留言板（/daemon/board/bug_board）用的轉接程式。
 * 設計文件：docs/WEB_BOARD.md
 *
 * 網頁在背景狀態查詢後面帶上請求：webhud board <動作> <請求代號> [參數…]，
 * 這裡把結果用 @@WEBHUD|B... 資料行回給網頁，玩家畫面上看不到。
 *   count <req>                          篇數、未讀數
 *   list <req>                           文章列表
 *   read <req> <編號> <時間>             讀一篇（並記為已讀）
 *   chunk <req> <序號> <內容>            發文內容分段送來（序號 0 表示新的一篇）
 *   post <req> <bug|idea> <標題>         用送來的內容發文
 *   del <req> <編號> <時間>              刪除
 * 標題與內容的跳脫：\\ 反斜線、\n 換行、\s 空白。
 * 文章的存放、篇數上限、刪除權限都照留言板原本的規則（std/bboard.c）。
 */

#define BOARD "/daemon/board/bug_board"
#define TITLE_MAX 24        // 網頁輸入框限 20 字；回覆會自動加「Re﹕」
#define BODY_MAX 1000
#define DRAFT_MAX 4000      // 跳脫後的長度上限，避免灌爆暫存

private void create() { seteuid(getuid()); }

private string field(string s)
{
    if (!stringp(s)) return "";
    s = replace_string(s, "|", "/");
    s = replace_string(s, "\r", "");
    s = replace_string(s, "\n", " ");
    return s;
}

// 內文：反斜線、直線、換行跳脫成一行。
private string escape_text(string s)
{
    if (!stringp(s)) return "";
    s = replace_string(s, "\\", "\\\\");
    s = replace_string(s, "|", "\\p");
    s = replace_string(s, "\r", "");
    s = replace_string(s, "\n", "\\n");
    return s;
}

private string unescape_text(string s)
{
    string out = "", c;
    int i;

    if (!stringp(s)) return "";
    while ((i = strsrch(s, "\\")) != -1) {
        if (i > 0) out += s[0..i-1];
        c = s[i+1..i+1];
        if (c == "n") out += "\n";
        else if (c == "s") out += " ";
        else out += c;
        s = s[i+2..];
    }
    return out + s;
}

private int char_count(string s)
{
    if (!stringp(s) || s == "") return 0;
    return sizeof(explode(s, ""));
}

private string trim(string s)
{
    while (sizeof(s) && (s[0] == ' ' || s[0] == '\n')) s = s[1..];
    while (sizeof(s) && (s[<1] == ' ' || s[<1] == '\n')) s = s[0..<2];
    return s;
}

private string author_of(object me)
{
    return me->query("name") + "(" + me->query("id") + ")";
}

private int is_admin(object me) { return wiz_level(me) >= 4; }

private mixed *read_list(object me)
{
    mixed r;
    if (!me->link()) return ({});
    r = me->link()->query("board_read/" + BOARD->query("board_id"));
    return arrayp(r) ? r : ({});
}

// 同 std/bboard.c 的 mark_read：記下讀過的留言時間，只保留還在板上的。
private void mark_read(object me, mapping *notes, int time)
{
    mixed *list = read_list(me), *keep = ({});
    int i;

    if (!me->link()) return;
    if (member_array(time, list) == -1) list += ({ time });
    for (i = 0; i < sizeof(notes); i++)
        if (member_array(notes[i]["time"], list) != -1) keep += ({ notes[i]["time"] });
    me->link()->set("board_read/" + BOARD->query("board_id"), keep);
}

private mapping *notes_of()
{
    mixed n = BOARD->query("notes");
    return arrayp(n) ? n : ({});
}

private int unread_count(object me, mapping *notes)
{
    mixed *list = read_list(me);
    int i, n;
    for (i = 0; i < sizeof(notes); i++)
        if (member_array(notes[i]["time"], list) == -1) n++;
    return n;
}

private void done(object me, string req, mapping *notes)
{
    printf("@@WEBHUD|BOARDEND|%s|ok|%d|%d|%d|%d\n", req, sizeof(notes), unread_count(me, notes),
        is_admin(me), BOARD->query("capacity"));
}

private void fail(string req, string msg)
{
    printf("@@WEBHUD|BOARDEND|%s|err|%s\n", req, field(msg));
}

// 用編號找留言，時間不符（別人剛刪文、編號移動了）就改用時間找。
private int find_note(mapping *notes, int num, int time)
{
    int i;
    if (num >= 1 && num <= sizeof(notes) && notes[num-1]["time"] == time) return num - 1;
    for (i = 0; i < sizeof(notes); i++)
        if (notes[i]["time"] == time) return i;
    return -1;
}

void handle(object me, string arg)
{
    string action, req, rest, cat, title, body, draft;
    mapping *notes, note;
    mixed *list;
    int i, num, time, seq;

    if (!me || !userp(me) || !stringp(arg)) return;
    if (sscanf(arg, "%s %s %s", action, req, rest) != 3) {
        if (sscanf(arg, "%s %s", action, req) != 2) return;
        rest = "";
    }
    req = field(req);
    write("@@WEBHUD|BOARDBEGIN|" + req + "\n");

    if (catch(notes = notes_of())) { fail(req, "留言板暫時無法使用。"); return; }

    switch (action) {
    case "count":
        done(me, req, notes);
        return;

    case "list":
        list = read_list(me);
        for (i = 0; i < sizeof(notes); i++)
            printf("@@WEBHUD|BNOTE|%d|%d|%d|%d|%s|%s\n", i + 1, notes[i]["time"],
                member_array(notes[i]["time"], list) != -1,
                notes[i]["author"] == author_of(me),
                field(notes[i]["author"]), field(notes[i]["title"]));
        done(me, req, notes);
        return;

    case "read":
        if (sscanf(rest, "%d %d", num, time) != 2 || (i = find_note(notes, num, time)) < 0) {
            fail(req, "這篇留言已經不在板上了。");
            return;
        }
        printf("@@WEBHUD|BREAD|%d|%d|%d|%s|%s|%s\n", i + 1, notes[i]["time"],
            notes[i]["author"] == author_of(me),
            field(notes[i]["author"]), field(notes[i]["title"]), escape_text(notes[i]["msg"]));
        mark_read(me, notes, notes[i]["time"]);
        done(me, req, notes);
        return;

    case "chunk":
        if (sscanf(rest, "%d %s", seq, body) != 2) {
            if (sscanf(rest, "%d", seq) != 1) { fail(req, "內容傳送失敗，請再試一次。"); return; }
            body = "";
        }
        draft = seq ? me->query_temp("web_board/draft") : "";
        if (!stringp(draft)) draft = "";
        draft += body;
        if (sizeof(draft) > DRAFT_MAX) { me->delete_temp("web_board/draft"); fail(req, "內容太長了。"); return; }
        me->set_temp("web_board/draft", draft);
        done(me, req, notes);
        return;

    case "post":
        if (BOARD->query("NO_PLAYER_POST") && !wizardp(me)) { fail(req, "這塊留言板只有巫師能張貼。"); return; }
        if (sscanf(rest, "%s %s", cat, title) != 2) { fail(req, "請填寫標題。"); return; }
        title = trim(unescape_text(title));
        body = trim(unescape_text(me->query_temp("web_board/draft")));
        me->delete_temp("web_board/draft");
        if (title == "") { fail(req, "請填寫標題。"); return; }
        if (char_count(title) > TITLE_MAX) { fail(req, "標題太長了，請在二十個字以內。"); return; }
        if (body == "") { fail(req, "請填寫內容。"); return; }
        if (char_count(body) > BODY_MAX) { fail(req, "內容太長了，請在一千個字以內。"); return; }
        if (cat == "idea") title = "【建議】" + title;
        else title = "【回報】" + title;
        note = allocate_mapping(4);
        note["title"] = field(title);
        note["author"] = author_of(me);
        note["time"] = time();
        // 照 std/bboard.c 的 done_post：加簽名、超過上限時刪掉最舊的一批、存檔
        // （不直接呼叫它，因為它會在玩家畫面印出「留言完畢」）。
        body += "\n";
        if (me->link() && stringp(draft = me->link()->query("signature")))
            body += "--\n" + draft;
        note["msg"] = body;
        notes += ({ note });
        if (sizeof(notes) > BOARD->query("capacity"))
            notes = notes[BOARD->query("capacity") / 4 .. BOARD->query("capacity")];
        BOARD->set("notes", notes);
        BOARD->save();
        mark_read(me, notes, note["time"]);
        done(me, req, notes);
        return;

    case "del":
        if (sscanf(rest, "%d %d", num, time) != 2 || (i = find_note(notes, num, time)) < 0) {
            fail(req, "這篇留言已經不在板上了。");
            return;
        }
        if (notes[i]["author"] != author_of(me) && !is_admin(me)) { fail(req, "這篇留言不是你寫的。"); return; }
        notes = notes[0..i-1] + notes[i+1..];
        BOARD->set("notes", notes);
        BOARD->save();
        done(me, req, notes);
        return;
    }
    fail(req, "不明的請求。");
}
