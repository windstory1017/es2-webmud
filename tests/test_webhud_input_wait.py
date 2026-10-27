"""網頁背景狀態查詢（webhud）在等待輸入時暫停（2026-10-24）。

伺服器在等待玩家輸入（選單、Yes/No、寫文章、巫師 edit）時送 @@WEBHUD|INPUT，
回到指令提示時送 @@WEBHUD|READY；網頁在兩者之間不送 webhud。
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / 'source' / 'upstream' / 'mudlib'
APP = (ROOT / 'web' / 'app.js').read_text(encoding='utf-8')


def read(rel):
    return (MUD / rel).read_text(encoding='utf-8')


def test_webhud_marks_web_client_on_body_and_link():
    src = read('cmds/usr/webhud.c')
    assert 'me->set_temp("webhud/client", 1);' in src
    assert 'me->link()->set_temp("webhud/client", 1);' in src


def test_input_prompt_announces_pause():
    src = read('feature/user/input.c')
    assert 'query_temp("webhud/client")' in src
    assert '@@WEBHUD|INPUT' in src


def test_editor_and_race_menu_announce_pause():
    assert '@@WEBHUD|INPUT' in read('adm/simul_efun/webhud.c')
    assert '#include "/adm/simul_efun/webhud.c"' in read('adm/obj/simul_efun.c')
    assert '@@WEBHUD|INPUT' in read('adm/daemons/logind.c')


def test_prompt_announces_resume():
    src = read('feature/char/message.c')
    assert '@@WEBHUD|READY' in src
    assert src.index('@@WEBHUD|READY') < src.index('receive("> ");')


def test_browser_obeys_pause_and_resume():
    assert "if(a[1]==='INPUT'){serverInputWait=true;continue;}" in APP
    assert "if(a[1]==='READY'){serverInputWait=false;continue;}" in APP
    ready = APP.split('function hudPollReady', 1)[1].split('\n}', 1)[0]
    assert 'if(serverInputWait)return false;' in ready
    close_line = next(x for x in APP.splitlines() if 'ws.onclose=e=>' in x)
    assert 'serverInputWait=false' in close_line
