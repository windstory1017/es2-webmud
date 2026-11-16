from pathlib import Path
import re
ROOT=Path(__file__).resolve().parents[1]
MUD=ROOT/'source/upstream/mudlib'

def text(p): return p.read_text(encoding='utf-8')

def test_numbered_menu_is_global_for_option_mappings():
    inp=text(MUD/'feature/user/input.c')
    cur=text(MUD/'adm/simul_efun/cursor.c')
    assert 'write (sprintf ("%d. %s\\n", pos++, label));' in inp
    assert '請輸入選項數字:' in inp
    assert 'sscanf (input, "%d", choice) == 1' in cur

def test_login_creation_uses_line_input_for_choice_menus():
    login=text(MUD/'adm/daemons/logind.c')
    for cb in ('confirm_id','confirm_relogin','confirm_reincarnate','get_race','get_gender'):
        assert re.search(r'input_to\s*\(\s*"'+cb+r'"', login)
    # v3.2 and later may migrate the remaining pager to line input too.
    hits=[]
    for p in MUD.rglob('*.c'):
        if p.name=='input.c': continue
        if 'get_char (' in text(p) or 'get_char(' in text(p): hits.append(p.relative_to(MUD).as_posix())
    assert set(hits).issubset({'feature/user/more.c'})

def test_password_prompts_remain_noecho_and_browser_secret():
    login=text(MUD/'adm/daemons/logind.c')
    assert 'input_to ("new_password", 1, ob);' in login
    assert 'input_to ("retype_password", 1, ob);' in login
    app=text(ROOT/'web/app.js')
    assert "const passwordPrompts=" in app
    # 密碼模式：送出指令時 sensitive 只在「非自動送出且輸入框是密碼」時成立，
    # 而且密碼分支在 rememberCommand（寫入指令歷史）之前就 return，密碼不會進指令歷史。
    assert "const sensitive=!auto&&input.type==='password'" in app
    submit=app.split("document.querySelector('#form')?.addEventListener('submit'",1)[1]
    assert submit.index("input.type==='password'") < submit.index('rememberCommand(raw)')
    assert "if(!sensitive)echoCommand(c)" in app

def test_save_permissions_are_scoped_to_own_canonical_paths():
    sec=text(MUD/'adm/daemons/securityd.c')
    assert 'func=="mkdir" && sizeof(path)==3' in sec
    assert '(path[1]=="user" || path[1]=="login" || path[1]=="mail")' in sec
    assert 'path[2]==euid[0..0]' in sec
    assert 'file==user_data(euid) || file==login_data(euid)' in sec
    assert 'file==mail_data(euid)' in sec

def test_disconnect_persists_body_and_link():
    user=text(MUD/'obj/user.c')
    block=user[user.index('private void net_dead()'):user.index('if( is_busy()', user.index('private void net_dead()'))]
    assert 'save();' in block
    # user->save() already saves its link object; v3.2 removes the redundant second write.
    assert 'destruct(my_link);' in block

def test_browser_keeps_text_input_without_duplicate_choice_buttons():
    html=text(ROOT/'web/index.html'); app=text(ROOT/'web/app.js')
    assert 'id="choicePanel"' not in html and 'id="choiceButtons"' not in html
    assert 'id="command"' in html
    assert "updateChoicePanel(s)" not in app
    assert "document.querySelector('#form')" in app
