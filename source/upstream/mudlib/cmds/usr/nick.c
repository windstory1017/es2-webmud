

#include <ansi.h>

inherit F_CLEAN_UP;

#define NICK_MAX_WIDTH 12   // 最多 12 格：6 個中文字，或 12 個英數字（中文字算 2 格）

int main(object me, string arg)
{
    string ch;
    int width;

    if( !arg ) return notify_fail("你要替自己取什麼綽號﹖\n");

    // 綽號只能是一行乾淨的文字：不可換行，也不可夾帶控制字元（顏色碼、跳格等）。
    foreach(ch in explode(arg, "")) {
	if( strlen(ch) == 1 && (ch[0] < ' ' || ch[0] == 127) )
	    return notify_fail("你的綽號不能換行，也不能夾帶特殊的控制字元。\n");
	width += (strlen(ch) > 1) ? 2 : 1;
    }
    if( width > NICK_MAX_WIDTH )
	return notify_fail("你的綽號太長了﹐最多六個中文字﹐想一個短一點的、響亮一點的。\n");

    seteuid(getuid());
    me->set("nickname", arg);    // use arg + NOR if colorful nick is open
    write("Ok。\n");
    return 1;
}

int help(object me)
{
    write(@HELP
指令格式 : nick <外號, 綽號>
 
這個指令可以讓你為自己取一個響亮的名號或頭銜。
綽號最多六個中文字（或十二個英文字母），只能是一行文字，不能換行。

你如果希望在綽號中使用 ANSI 的控制字元改變顏色﹐可以用以下的控制字串﹕
(暫不開放)

$BLK$ - 黑色	$NOR$ - 恢復正常顏色
$RED$ - 紅色	$HIR$ - 亮紅色
$GRN$ - 綠色	$HIG$ - 亮綠色
$YEL$ - 土黃色	$HIY$ - 黃色
$BLU$ - 深藍色	$HIB$ - 藍色
$MAG$ - 淺紫色	$HIM$ - 粉紅色
$CYN$ - 藍綠色	$HIC$ - 天青色
$WHT$ - 淺灰色	$HIW$ - 白色

HELP
        );
        return 1;
}

