from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def text(p, enc="utf-8"): return (ROOT/p).read_text(encoding=enc, errors="replace")

def test_page_connects_without_landing_cover():
    html=text("web/index.html")
    js=text("web/app.js")
    assert 'id="landing"' not in html
    assert 'id="gameApp" class="game-app">' in html
    assert "bootPreviewMode().then(()=>startGame());" in js

def test_hud_poll_is_silent_and_colored():
    js=text("web/app.js")
    assert "hudPollInFlight" in js
    assert "return {visible:'',done:false}" in js
    assert "consumeHudPollOutput" in js
    assert "const colors={hp:'#2f7a4a'" in js  # 右欄角色狀態的顏色跟 score 一致
    assert "hud-shen" not in js  # class is generated dynamically by key, not hard-coded markup
    css=text("web/styles.css")
    assert "--hud-color" in css
    assert "::-webkit-progress-value" in css

def test_welcome_is_presentation_styled_only():
    js=text("web/app.js")
    assert "renderWelcomeIfPresent" not in js
    welcome=text("source/upstream/mudlib/adm/etc/welcome")
    assert "東 方 故 事 Ⅱ" in welcome
    assert "github.com/taedlar/es2_mudlib" in welcome
    assert "fangyan <gbfangyans@gmail.com>" in welcome

def test_launchers_are_separated():
    start=text("START_ES2.bat", "ascii")
    opener=text("OPEN_ES2_BROWSER.bat", "ascii")
    assert 'start_es2_windows.ps1' in start
    assert 'start_es2_windows.ps1' in start
    assert 'http://127.0.0.1:8080' not in start
    assert 'http://127.0.0.1:8080' in opener

def test_quick_action_panel_removed():
    # 快捷操作欄已移除（2026-10-27），手動存檔也不能回來。
    html=text("web/index.html")
    assert '快捷操作' not in html
    assert '<button data-cmd="save">手動存檔</button>' not in html
