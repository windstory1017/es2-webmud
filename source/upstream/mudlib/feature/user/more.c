/*---
description: text pager, used to display long text in pages.
author: Annihilator <taedlar@gmail.com>
---*/
#include <ansi.h>

private int i, matched, match_line, tmp_line;
private string pattern;

private int justify_next_line (int line, int total, int page_size) {
    if (line < 0)
        line = 0; // first page
    else if (line > total - page_size)
        line = total - page_size; // last page
    return line;
}

/* [NEOLITH-EXTENSION]
 *
 * The `input_prompt` apply will recognize the pager by callback function name "more"
 * and write the pager prompt to the user.
 */
void more (string cmd, string *text, int line) {
    int page_size = this_object()->query("page_size") || 22;

    /* HELP pager: only next/previous are pager controls.  Anything else is a
     * normal player command, so HELP never traps the character in a menu. */
    switch (cmd) {
    case "2": case "b": case KEY_PAGE_UP:
        more (" ", text, justify_next_line (line - 2*page_size, sizeof(text), page_size));
        return;
    case "": case "1": case " ": case KEY_PAGE_DOWN:
        line = justify_next_line (line, sizeof(text), page_size);
        write (HOME CLR + implode (text[line .. line + page_size - 1], "\n"));
        line += page_size;
        input_to ("more", text, line);
        return;
    case "webhud":
        /* 網頁背景狀態查詢：照常回傳狀態，但分頁停在原處，不要因此結束。 */
        command(cmd);
        input_to ("more", text, line);
        return;
    default:
        /* Release input_to before dispatching, otherwise the next command can
         * be swallowed by the pager again. */
        if (stringp(cmd) && cmd != "")
            command(cmd);
        return;
    }
}
/*
 * 分段送出：遊戲伺服器一次只能印 8192 個字元（位元組），超過會出錯、整段都看不到
 * （例如 more 一個超過 8KB 的檔案）。這裡把多行文字湊成每段不超過 4096 字元再送。
 */
private void write_lines (string *lines) {
    string buf = "";

    foreach (string line in lines) {
        if (strlen (buf) + strlen (line) + 1 > 4096 && buf != "") {
            write (buf);
            buf = "";
        }
        buf += line + "\n";
    }
    if (buf != "")
        write (buf);
}

/*
 * Start the pager with the given text.
 *
 * WebMUD：取消強制分頁，長文章一次顯示完整（使用者核准）。網頁終端機可以自己捲動，
 * 分頁反而會被背景狀態查詢打斷。上面的 more() 保留給仍直接呼叫它的程式。
 */
varargs void start_more (mixed msg, int start_line) {
    if (stringp (msg))
        msg = explode (msg, "\n");
    else if (!arrayp (msg))
        error ("Invalid argument to start_more: expected string or array of strings.");
    if (start_line > 0 && start_line < sizeof (msg))
        msg = msg[start_line..];
    write_lines (msg);
}

void start_more_if_needed (mixed msg) {
    if (stringp (msg))
        msg = explode (msg, "\n");
    else if (!arrayp (msg))
        error ("Invalid argument to start_more_if_needed: expected string or array of strings.");
    write_lines (msg);
}
