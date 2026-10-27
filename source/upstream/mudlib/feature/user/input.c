/*---
description: Input prompt for get_char() and input_to() carryover mapping.
author: Annihilator <taedlar@gmail.com>
---*/

#include <ansi.h>

/* [NEOLITH-EXTENSION]
 * Render option mappings as a browser/telnet-friendly numbered list.
 *
 * LOGIN_D uses ordinary input_to() for these menus, so players enter a
 * number and press Enter. cursor_translate() maps that number back to the
 * original canonical option value. This avoids VT100 cursor movement and
 * get_char() single-character transport during login/character creation.
 *
 * The pager branch below still retains its original get_char() controls.
 */
void input_prompt (mixed func, int flags, mixed data, mixed arg1) {
    /* 網頁玩家進入等待輸入（選單、Yes/No、寫文章、翻頁……）：通知網頁暫停背景狀態查詢，
     * 否則查詢會被當成玩家的回答。回到指令提示時由 write_prompt() 通知恢復。 */
    if (this_object()->query_temp("webhud/client")) {
        this_object()->set_temp("webhud/input_wait", 1);
        receive ("\n@@WEBHUD|INPUT\n");
    }

    if (mapp(data) && arrayp(data["options"]) && intp(data["cursor"])) {
        string prompt = data["prompt"] || "你的選擇：";
        int pos = 1;

        /* Binary yes/no and male/female prompts collapse onto the same line
         * as the question itself -- "...嗎？(Yes / No) " -- instead of the
         * numbered-menu block below, which is reserved for lists with more
         * than two options (race selection etc). Checked first so the
         * generic "\n"+prompt+"\n" write below never fires for these. */
        if (sizeof(data["options"]) == 2
        &&  data["options"][0] == "Y) 是"
        &&  data["options"][1] == "N) 否") {
            write ("\n" + prompt + "(Yes / No) ");
            return;
        }
        if (sizeof(data["options"]) == 2
        &&  data["options"][0] == "M) 男性"
        &&  data["options"][1] == "F) 女性") {
            write ("\n" + prompt + "(Male / Female) ");
            return;
        }

        /* Browser/telnet compatible numbered menu. No VT100 cursor movement is
         * required: enter 1, 2, 3... and press Enter. The callback still uses
         * cursor_translate(), so canonical option values and rules are unchanged. */
        write ("\n" + prompt + "\n");

        foreach (string opt in data["options"]) {
            string label = opt;
            if (opt == "human")
                label = "人類 (human)";
            else if (opt == "avatar")
                label = "人類族 (avatar)";
            else if (opt == "blackteeth")
                label = "黑齒 (Blackteeth)";
            else if (opt == "yenhold")
                label = "厭火 (Yenhold)";
            else if (opt == "jiaojao")
                label = "焦僥 (Jiaojao)";
            else if (opt == "woochan")
                label = "無腸 (Woochan)";
            else if (opt == "dingling")
                label = "釘靈 (Dingling)";
            else if (opt == "headless")
                label = "刑天 (Headless)";
            else if (opt == "rainner")
                label = "雨師妾 (Rainner)";
            else if (opt == "malik")
                label = "巫首 (Malik)";
            else if (opt == "yaksa")
                label = "夜叉 (Yaksa)";
            else if (opt == "ashura")
                label = "阿修羅 (Ashura)";
            else if (opt.len() > 2 && opt[1] == ')') {
                if (opt.len() > 3 && opt[2] == ' ')
                    label = opt[3..];
                else
                    label = opt[2..];
            }
            write (sprintf ("%d. %s\n", pos++, label));
        }
        write ("請輸入選項數字: ");
    }
    else if (func == "more" && arrayp (data) && intp (arg1)) {
        int page_size = this_object()->query("page_size") || 22;
        if (arg1 < 0)
            arg1 = 0;
        else if (arg1 > sizeof (data))
            arg1 = sizeof (data);
        // The pager has two arguments: the text array and the current line number.
        {
            int total_pages = (sizeof(data) + page_size - 1) / page_size;
            int current_page = (arg1 + page_size - 1) / page_size;
            if (current_page < 1) current_page = 1;
            if (current_page > total_pages) current_page = total_pages;
            printf (
                "\n" CLR BGRN "【 第 %d / %d 頁 】\n" NOR
                "1. 下一頁\n"
                "2. 上一頁\n"
                "請輸入選項數字，或直接輸入其他指令: ",
                current_page, total_pages
            );
        }
    }
}
