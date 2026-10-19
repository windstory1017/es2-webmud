/*---
description: UTF-8 utilities
author: Annihilator <taedlar@gmail.com>
---*/

// 不能出現在行首的全形收尾標點（cjk_wrap() 換行時讓它們留在上一行）。
#define NO_LINE_START ({ "﹗", "！", "？", "﹖", "，", "﹐", "。", "﹒", "、", "；", "﹔", "：", "﹕", \
    "」", "』", "）", "】", "〉", "》", "…" })

varargs string cjk_wrap (string str, int width, int indent, int first_line_indent) {
    string* mbch;
    string result = "";
    int line_length = first_line_indent;
    int prev_char_width = 0;

    if (!width || width < 1)
        return str;
    mbch = explode (str, ""); // [NEOLITH-EXTENSION] explode to array of utf-8 characters
    foreach (string ch in mbch) {
        int char_width = (strlen (ch) > 1) ? 2 : 1; // CJK multi-byte chars are double width
        if (ch == "\n" || ch == "\r") {
            if (indent < 0) {
                result += ch;
                line_length = 0;
                continue; // preserve existing newlines if indent is negative
            }
            if (prev_char_width > 1)
                continue; // skip newlines immediately following multi-byte characters to avoid breaking them
            ch = " "; // treat newlines as spaces for wrapping purposes
        }
        prev_char_width = char_width;
        if (char_width > 1 || ch == " ") { // break on spaces and multi-byte characters
            // 收尾的標點（﹗。」等）不放在行首，超出寬度時掛在上一行尾端。
            if (line_length + char_width > width && member_array (ch, NO_LINE_START) == -1) {
                result += "\n";
                if (indent > 0)
                    result += repeat_string (" ", indent);
                line_length = indent;
            }
        }
        if (ch == " " && line_length == 0)
            continue; // skip leading spaces
        result += ch;
        if (ch == "\t")
            line_length += 8 - (line_length % 8); // tab stops every 8 characters
        else
            line_length += char_width;
    }

    return result;
}

// cjk_pad()
// NEW（非原始 ES2 內容，用來讓中英夾雜文字對齊成整齊欄位）
//
// Pads str with spaces to reach `width` visual columns, counting CJK
// multi-byte characters as width 2 (matching cjk_wrap()'s own rule) instead
// of raw byte/character count, so %-Ns style sprintf padding does not go
// ragged when a string mixes Chinese and ASCII. If str is already >= width,
// it is returned unchanged (never truncated). right_align pads on the left
// instead of the right.
varargs string cjk_pad (string str, int width, int right_align) {
    string* mbch;
    int w, pad;

    int in_ansi = 0;

    mbch = explode (str, "");
    w = 0;
    // ANSI 顏色碼（ESC[...m）不佔畫面寬度：有顏色的商品名稱原本被算得太寬，後面的價格欄就對不齊。
    foreach (string ch in mbch) {
        if (in_ansi) {
            if (ch == "m") in_ansi = 0;
            continue;
        }
        if (ch == "\x1b") {
            in_ansi = 1;
            continue;
        }
        w += (strlen (ch) > 1) ? 2 : 1;
    }

    pad = width - w;
    if (pad <= 0) return str;

    return right_align
        ? repeat_string (" ", pad) + str
        : str + repeat_string (" ", pad);
}

// cjk_align()
// NEW（非原始 ES2 內容，讓中英夾雜的欄位在任何字型下都能對齊）
//
// 網頁版的中文字與英文字常來自不同字型，中文字寬不一定剛好等於兩個英文字，
// 只用「中文算兩格」補空白（cjk_pad）時，中文字越多的行仍會偏移。這個函式
// 把 str 的全形字數補到 wide 個（用全形空白「　」），半形字數補到 narrow 個
// （用半形空白），每一行的全形字數與半形字數都相同，不論字型都會對齊。
// 全形字：U+2E80 以後的字元（中日韓文字、全形符號、全形空白等）；
// 其餘（英數、ˇ、• 等）算半形。任一部分已經超過時不截斷。
string cjk_align (string str, int wide, int narrow) {
    int w = 0, n = 0;

    foreach (string ch in explode (str, "")) {
        if (strlen (ch) > 1 && ch >= "⺀") w++;
        else n++;
    }

    if (wide > w) str += repeat_string ("　", wide - w);
    if (narrow > n) str += repeat_string (" ", narrow - n);
    return str;
}
