// guard.c

inherit F_CLEAN_UP;

int main(object me, string arg)
{
        object ob;

	if( me->query("life_form") == "ghost" )
		return notify_fail("人死了就一了百了, 快去找城隍復活吧!!\n");

        if( !arg || arg=="" ) {
                if( ob = me->query_guarding() ) {
                        write("你現在保護的對象是" + ob->name() + "。\n");
                        return 1;
                }
                write("你現在並沒有選定保護的對象。\n");
                return 1;
        }

        if( sscanf(arg, "cancel %s", arg) ) {
                ob = present(arg, environment(me));
                if( !ob || ob->query_temp("guard_owner") != getuid(me) )
                        return notify_fail("你並沒有看守這樣東西。\n");
                ob->delete_temp("guard_owner");
                write("你不再看守" + ob->name() + "。\n");
                return 1;
        }

        if( arg=="cancel" ) {
                write("Ok.\n");
                me->guard_ob(0);
                return 1;
        }

        ob = present(arg, environment(me));
        if( !ob ) return notify_fail("這裡沒有這個人，也沒有這樣東西。\n");
        if( !living(ob) ) {
                string owner;

                if( environment(ob) != environment(me) )
                        return notify_fail("你只能看守地上的東西。\n");
                owner = ob->query_temp("guard_owner");
                if( owner == getuid(me) )
                        return notify_fail("你已經在看守" + ob->name() + "了。\n");
                if( owner && find_player(owner) && environment(find_player(owner)) == environment(me) )
                        return notify_fail(find_player(owner)->name() + "正看守著" + ob->name() + "。\n");
                if( ob->query("no_get") )
                        return notify_fail(ob->name() + "不需要你看守。\n");
                ob->set_temp("guard_owner", getuid(me));
                write("你開始看守著" + ob->name() + "，除了你沒有人能拿走。\n");
                return 1;
        }
        if( ob==me ) return notify_fail("你「理所當然」的會保護自己。\n");
        if( !userp(ob) )
                return notify_fail("他不需要你的保護。\n");
        if( me->query("level") < 5)
                return notify_fail("等你長大點再來保護別人\n");
        if( me->is_fighting(ob) )
                return notify_fail("你想保護正在跟你戰鬥的對手？\n");

        me->guard_ob(ob);
        write("Ok.\n");

        return 1;
}

int help(object me)
{
        write(@TEXT
指令格式﹕guard [<人物>|<物品>|cancel [<物品>]]

你可以用這個指令設定你要保護的對象，當你保護的對象遭受攻擊時，你的人物會在
第一時間加入戰鬥助戰。

guard <物品> 可以看守地上的東西（容器也可以）：看守期間只有你能拿走，別人
拿取會被擋下；容器被看守時，別人拿不走容器，也拿不出裡面的東西，但依然可以
從裡面取水來喝。你離開房間後看守暫時失效，回到房間又會自動生效，不必重下指
令。用 guard cancel <物品> 可以取消看守。

要取消 guard 模式﹐可以用 guard cancel﹐不加任何參數則顯示你目前所保護的對
象‧
TEXT
        );
        return 1;
}
