from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
APP = (ROOT / 'web' / 'app.js').read_text(encoding='utf-8')
NOTE = ROOT / 'reference' / 'user' / '升級公式_說明.md'
XLSX = ROOT / 'reference' / 'user' / '升級公式.xlsx'


def _button_block():
    start = APP.index('function bindCommandButtons(root)')
    end = APP.index('function bindContextActions()', start)
    return APP[start:end]


def test_interaction_buttons_use_shared_send_pipeline():
    block = _button_block()
    assert 'send(command);' in block
    assert "ws.send(command+'\\r\\n')" not in block
    assert 'sessionRecorder.beginCommand(command)' not in block


def test_inventory_and_interactions_bind_through_same_button_handler():
    # 互動面板已拿掉，只剩「可使用道具」面板，仍用同一個按鈕處理函式
    assert 'function bindContextActions(){bindCommandButtons(contextBody);}' in APP


def test_user_upgrade_formula_is_preserved_for_followup():
    assert XLSX.exists() and XLSX.stat().st_size > 0
    text = NOTE.read_text(encoding='utf-8')
    assert '升級公式.xlsx' in text
    assert '左手劍法' in text
    assert '左手刀法' in text
