from pathlib import Path
R=Path(__file__).parents[1]
html=(R/'web/index.html').read_text(encoding='utf-8')
js=(R/'web/app.js').read_text(encoding='utf-8')
css=(R/'web/styles.css').read_text(encoding='utf-8')
inputc=(R/'source/upstream/mudlib/feature/user/input.c').read_text(encoding='utf-8')
assert 'id="landing"' not in html and '進入遊戲' not in html
assert 'id="localMap"' in html and '空白欄位' not in html and '快捷操作' not in html
assert 'choicePanel' not in html
assert 'grid-template-columns:repeat(7' in css
assert 'currentRoomId' in js and 'radiusX=Math.ceil(W/2/mapZoom/sx)' in js  # 地圖範圍跟著視窗大小（2026-10-27）
assert 'xterm256' in js
assert '(Yes / No) ' in inputc
print('v3.3 UI source checks passed')
