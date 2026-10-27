from pathlib import Path
R=Path(__file__).parents[1]
html=(R/'web/index.html').read_text(encoding='utf-8')
js=(R/'web/app.js').read_text(encoding='utf-8')
css=(R/'web/styles.css').read_text(encoding='utf-8')
inputc=(R/'source/upstream/mudlib/feature/user/input.c').read_text(encoding='utf-8')
assert 'id="landing"' not in html and '進入遊戲' not in html
assert 'id="localMap"' in html and html.count('空白欄位') == 3
assert 'choicePanel' not in html
assert 'grid-template-columns:repeat(7' in css
assert 'currentRoomId' in js and 'radius=3' in js
assert 'xterm256' in js
assert '(Yes / No) ' in inputc
print('v3.3 UI source checks passed')
