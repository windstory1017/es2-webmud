// name.c — 替繡花布衣命名（鳳娘白甌繡過的衣服）。設計見 docs/EMBROIDERY.md（使用者指示，2026-11-13）。

#include <ansi.h>
#include <daemon.h>

inherit F_CLEAN_UP;

#define NAME_MAX_WIDTH 12   // 最多 12 格：6 個中文字，或 12 個英數字（中文字算 2 格）

// 指令檔要有 euid 才能載入字表（BIG5_D）。
private void create() { seteuid(getuid()); }

int main(object me, string arg)
{
    string item, newname, ch;
    object ob;
    int width;

    if( !arg || sscanf(arg, "%s as %s", item, newname) != 2 || item == "" || newname == "" )
        return notify_fail("指令格式 : name <繡花布衣> as <名稱>\n");

    // 身上可能同時有普通布衣與繡花布衣（id 都叫 cloth），只挑繡花過的。
    foreach(object o in all_inventory(me))
        if( o->id(item) && o->query("embroidered") && function_exists("set_custom_name", o) ) {
            ob = o;
            break;
        }
    if( !objectp(ob) ) {
        if( !objectp(present(item, me)) )
            return notify_fail("你身上沒有這樣東西。\n");
        return notify_fail("只有白甌繡過的繡花布衣才能取名字。\n");
    }

    // 名稱只能是一行乾淨的文字：不可換行，不可夾帶顏色碼與控制字元，頭尾不留空白。
    if( newname[0] == ' ' || newname[<1] == ' ' )
        return notify_fail("名稱的頭尾不能留空白。\n");
    foreach(ch in explode(newname, "")) {
        if( strlen(ch) == 1 && (ch[0] < ' ' || ch[0] == 127 || ch == "$" || ch == "%" || ch == "|") )
            return notify_fail("名稱不能換行，也不能夾帶顏色碼或特殊字元。\n");
        width += (strlen(ch) > 1) ? 2 : 1;
    }
    if( width > NAME_MAX_WIDTH )
        return notify_fail("名稱太長了﹐最多六個中文字。\n");
    if( !BIG5_D->valid_text(newname) )
        return notify_fail("名稱裡有些字太罕見了﹐請換成常用的字。\n");

    ob->set_custom_name(newname);
    write("你替這件衣服取了個名字﹐從此它就叫做「" + newname + "」。\n");
    return 1;
}

int help(object me)
{
    write(@HELP
指令格式 : name <繡花布衣> as <名稱>

這個指令可以替白甌繡過的繡花布衣取一個名字，例如 name cloth as 流雲。
可以隨時改名，改幾次都行。名稱最多六個中文字（或十二個英文字母），
只能是一行文字，不能含顏色碼，也不能用太罕見的字。
改名後衣服的能力不變，英文名稱仍是 Embroidered cloth。

HELP
    );
    return 1;
}
