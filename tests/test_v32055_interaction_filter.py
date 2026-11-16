from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
HUD=(ROOT/'source/upstream/mudlib/cmds/usr/webhud.c').read_text()
INN=(ROOT/'source/upstream/mudlib/d/snow/inn_hall.c').read_text()

def test_no_generic_detail_enumeration():
    assert 'details = room->query("detail")' not in HUD
    assert 'foreach (id, desc in details)' not in HUD

def test_hud_is_read_only_status_feed_without_room_action_registry():
    # 早期版本讓房間／物件用 web_actions 登記網頁按鈕，現在已拿掉；HUD 只提供唯讀的狀態與物品。
    assert 'Read-only status/inventory feed' in HUD
    assert 'web_actions' not in HUD

def test_safe_room_still_blocks_fighting():
    assert 'set ("no_fight",1);' in INN
