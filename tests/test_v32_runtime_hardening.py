from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
MUD=ROOT/'source/upstream/mudlib'
def text(p): return p.read_text(encoding='utf-8-sig' if p.suffix=='.ps1' else 'utf-8')

def test_pager_is_numbered_line_mode_and_has_no_raw_get_char():
    more=text(MUD/'feature/user/more.c')
    inp=text(MUD/'feature/user/input.c')
    assert 'input_to ("more", text, line);' in more
    assert 'get_char ("more"' not in more
    # 現在的說明翻頁只有「下一頁（1）」「上一頁（2）」兩個選項，其他輸入一律當一般指令，不會把人卡在選單裡
    for n in ('case "1"','case "2"'):
        assert n in more
    assert 'default:' in more and 'command(cmd)' in more
    assert '1. 下一頁' in inp and '2. 上一頁' in inp
    assert '請輸入選項數字，或直接輸入其他指令: ' in inp

def test_online_autosave_and_disconnect_single_write_path():
    user=text(MUD/'obj/user.c')
    hb=user[user.index('private void heart_beat()'):user.index('void setup()', user.index('private void heart_beat()'))]
    assert 'last_auto_save' in hb
    assert '>= 300' in hb
    assert 'if (save())' in hb
    nd=user[user.index('private void net_dead()'):user.index('if( is_busy()', user.index('private void net_dead()'))]
    assert nd.count('save();') == 1
    assert 'my_link->save();' not in nd

def test_browser_strips_cursor_control_and_marks_password_mode():
    app=text(ROOT/'web/app.js')
    html=text(ROOT/'web/index.html')
    assert 'function terminalSafe' in app
    assert "m.endsWith('m')?m:''" in app
    assert "mode.textContent=secret?'密碼輸入':'指令輸入'" in app
    assert 'id="inputMode"' in html

def test_one_click_start_waits_for_both_services_and_avoids_duplicates():
    ps=text(ROOT/'start_es2_windows.ps1')
    bat=(ROOT/'START_ES2.bat').read_text(encoding='ascii')
    assert 'Test-Port $MudPort' in ps
    assert 'Wait-Port $MudPort 20' in ps
    assert 'Test-Port $WebPort' in ps
    assert 'Wait-Port $WebPort 20' in ps
    assert 'Start-Process ("http://127.0.0.1:"+$WebPort)' not in ps
    assert 'OPEN_ES2_BROWSER.bat' in ps
    assert 'start_es2_windows.ps1' in bat
    assert 'start_es2_windows.ps1' in bat
    assert 'http://127.0.0.1:8080' not in bat
    assert 'http://127.0.0.1:8080' in (ROOT/'OPEN_ES2_BROWSER.bat').read_text(encoding='ascii')

def test_save_checker_validates_login_and_body_files():
    ps=text(ROOT/'check_player_save_windows.ps1')
    assert 'login\\' in ps and 'user\\' in ps
    assert '[MISSING]' in ps and '[OK]' in ps
