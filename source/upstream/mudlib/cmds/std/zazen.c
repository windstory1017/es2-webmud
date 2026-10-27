// zazen：打坐參習善想禪要（使用者定案，2026-10-19）。實際內容見 /daemon/skill/absorption.c。

inherit F_CLEAN_UP;

private void create() { seteuid(getuid()); }

int main(object me, string arg)
{
    return SKILL_D("absorption")->start_zazen(me);
}

int help(object me)
{
    write(@HELP
指令格式﹕zazen

盤膝打坐﹐參習善想禪要。只有佛門弟子能夠打坐。
打坐期間會持續耗神﹐神不足時便會出定﹔也可以隨時停下。
HELP
    );
    return 1;
}
