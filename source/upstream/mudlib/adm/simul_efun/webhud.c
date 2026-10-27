// 網頁玩家進入線上編輯器（巫師 edit）時，通知網頁暫停背景狀態查詢（webhud），
// 否則查詢會被當成一行內容寫進檔案。離開編輯器回到指令提示時，
// feature/char/message.c 的 write_prompt() 會通知網頁恢復。
// 其他等待輸入（input_to、get_char）由 feature/user/input.c 的 input_prompt() 處理。

varargs void ed(string file, mixed exit_fn, int restricted)
{
    object me = this_player();

    if (objectp(me) && me->query_temp("webhud/client")) {
        me->set_temp("webhud/input_wait", 1);
        tell_object(me, "\n@@WEBHUD|INPUT\n");
    }
    if (undefinedp(exit_fn)) efun::ed(file);
    else if (undefinedp(restricted)) efun::ed(file, exit_fn);
    else efun::ed(file, exit_fn, restricted);
}
