

inherit F_CLEAN_UP;

void create() { seteuid(getuid()); }

int main(object me, string arg)
{
    string *tuned_ch;
    // 可以用 tune 開關、但玩家不能主動發聲的廣播頻道（例如全服公告 event）。
    // 這類頻道沒有「講話」指令，無法靠對它講話打開，所以要在這裡特別處理。
    string *broadcast_ch = ({ "event" });

    tuned_ch = me->link()->query("channels");
    if( !arg ) {
        if( !pointerp(tuned_ch) || !sizeof(tuned_ch) )
            write("你現在並沒有收聽任何頻道。\n");
        else
            write("你現在收聽的頻道﹕" + implode(tuned_ch, ", ") + "。\n");
        return 1;
    }

    if( pointerp(tuned_ch) && member_array(arg, tuned_ch)!=-1 ) {
        write("關閉 " + arg + " 頻道。\n");
        tuned_ch -= ({ arg });
        me->link()->set("channels", tuned_ch);
        return 1;
    } else if( member_array(arg, broadcast_ch) != -1 ) {
        // 廣播頻道不能用講話打開，直接在這裡開啟。
        if( !pointerp(tuned_ch) ) tuned_ch = ({});
        write("打開 " + arg + " 頻道。\n");
        me->link()->set("channels", tuned_ch + ({ arg }));
        return 1;
    } else {
        write("要打開某個頻道只要用該頻道說話即可。\n");
        return 1;
    }
}

int help(object me)
{
    write(@HELP
指令格式﹕tune [<頻道名稱>]

這個指令讓你選擇是否要收聽某一頻道的訊息﹐如果沒有指定頻道名稱﹐就會列出
你目前收聽中的頻道﹐如果指定了頻道﹐原來收聽中的就會關掉﹐反之打開。

如果你對一個沒有收聽中的頻道講話﹐會自動將它打開。
像 event（全服公告）這類不能講話的頻道﹐直接用 tune event 就能開關。
要對一個頻道講話﹐只要用﹕

<頻道名稱> <訊息> ....

例子﹕
  chat hello everyone!
  
你可以用 help channels 查詢目前有哪些頻道可以收聽。

see also : shout
HELP
    );
    return 1;
}


