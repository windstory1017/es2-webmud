"""網頁回報板（docs/WEB_BOARD.md）：網頁、狀態查詢、轉接程式三邊要對得上。"""
from pathlib import Path
R = Path(__file__).parents[1]
MUD = R / 'source/upstream/mudlib'
html = (R / 'web/index.html').read_text(encoding='utf-8')
app = (R / 'web/app.js').read_text(encoding='utf-8')
board = (R / 'web/board.js').read_text(encoding='utf-8')
hud = (MUD / 'cmds/usr/webhud.c').read_text(encoding='utf-8')
bridge = (MUD / 'daemon/misc/web_board.c').read_text(encoding='utf-8')
bug = (MUD / 'daemon/board/bug_board.c').read_text(encoding='utf-8')

assert 'data-ui-board' in html and 'id="boardWindow"' in html
assert "import {setupBoard} from './board.js';" in app
assert "'webhud '+extra.cmd" in app            # 留言板請求跟著狀態查詢送出
assert 'board.onHud(a)' in app
assert '"/daemon/misc/web_board"->handle(me, arg[6..])' in hud
for action in ('count', 'list', 'read', 'chunk', 'post', 'del'):
    assert f'case "{action}":' in bridge, action
    assert f"request('{action}'" in board, action
for tag in ('BOARDBEGIN', 'BNOTE', 'BREAD', 'BOARDEND'):
    assert tag in bridge and tag in board, tag
assert 'set("capacity", 100);' in bug and '問題回報與建議' in bug
assert (R / 'docs/WEB_BOARD.md').exists()
print('web board checks passed')
