

#include <command.h>

inherit F_CLEAN_UP;

int main(object me, string arg)
{
    object link;

    SECURED_WIZARD_COMMAND;

    seteuid(getuid());
    if( (me!=this_player(1)) || !objectp(link = me->link()) ) return 0;

    if( !arg ) 
        link->set("invis", (int)link->query("invis") == 0 );
    else if( arg=="on" )
        link->set("invis", 1);
    else if( arg=="off" )
        link->set("invis", 0);
    else
        return notify_fail("指令格式﹕invis [on|off]\n");

    // 判斷看不看得到的程式（std/char.c 的 visible）查的是角色身上的設定，
    // 所以角色身上也要記一份，隱身才會生效（#29，2026-11-04）。
    me->set("invis", link->query("invis"));

    write("Ok。\n");
    return 1  ;
}

int help()
{
    write(@TEXT
指令格式﹕invis [on|off]

打開或關閉巫師的隱形功能。
TEXT
    );
    return 1;
}


