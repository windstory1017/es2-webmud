

inherit F_CLEAN_UP;

int main(object me, string arg)
{
    string term, txt;
    int g;
    mapping score_g, skill_g;

    score_g = me->query_score_gain();
    me->reset_score_gain();
    skill_g = me->query_skill_gain();
    me->reset_skill_gain();

    txt = "從你上次使用 gain 指令，到現在為止，你共獲得：\n\n";
    
    txt += "經驗點數：\n";
    if( !sizeof(score_g) )
	txt += "    無。\n";
    else {
	foreach(term, g in score_g)
	    // 名稱照畫面寬度補空格（中文算兩格），點數靠右對齊（#43，2026-11-09）。
	    txt += "    " + cjk_pad(to_chinese("score of " + term), 16)
		+ sprintf("%+8d 點\n", score_g[term]);
    }

    txt += "\n技能點數：\n";
    if( !sizeof(skill_g) )
	txt += "    無。\n";
    else {
	foreach(term, g in skill_g)
	    txt += "    " + cjk_pad(to_chinese(term), 16)
		+ sprintf("%+8d 點\n", skill_g[term]);
    }

    write(txt);

    // 這段時間有拿到點數的技能，累積夠了就升一級。
    me->apply_gain_progression(skill_g);

    // 各項經驗都達到升級所需時，人物等級提升一級。
    me->try_level_up();
    return 1;
}

int help()
{
    write(@TEXT
指令格式：gain

這個指令可以用來檢驗你的人物成長，每次你的人物獲得任何經驗點數或技能點數，系
統會記錄所獲得的值，當你使用 gain 指令，會顯示這些值，然後清除這些紀錄從頭開
始，你可以在戰鬥開始前先用 gain 清除，然後在戰鬥結束後用 gain 檢視你的人物從
剛剛的戰鬥中獲的多少進步。

技能點數只會累積，技能等級要在使用 gain 時才會提升：這段時間有獲得點數的技能，
累積的點數足夠升級就提升一級，每次 gain 最多一級。尚未學成的技能要累積到學成
所需的點數，才會在 gain 時一次練成。

人物等級也一樣：各項經驗點數達到升級所需時不會自動升級，要在使用 gain 時才會提
升一級，每次 gain 最多一級。
TEXT
    );
    return 1;
}

