#!/usr/bin/env python3
import argparse,json,re
from pathlib import Path

ap=argparse.ArgumentParser(description="Validate a captured ES2 login transcript against confirmed canonical prompts.")
ap.add_argument("transcript")
a=ap.parse_args()
text=Path(a.transcript).read_text(encoding="utf-8",errors="replace")

checks={
  "password_prompt":"請輸入密碼:" in text,
  "new_password_prompt":"請設定您的密碼:" in text,
  "confirm_password_prompt":"請再輸入一次您的密碼" in text,
  "email_prompt":"您的電子郵件地址" in text,
  "race_prompt":("選擇你的角色所屬的種族:" in text) or ("可以選擇以下的種族" in text),
  "display_name_prompt":"您的顯示名稱:" in text,
}
# A transcript may represent existing-login OR new-character flow.
existing=checks["password_prompt"]
new_character=all(checks[k] for k in ["new_password_prompt","confirm_password_prompt","email_prompt","race_prompt","display_name_prompt"])
out={"ok":existing or new_character,"flow":"existing" if existing else ("new_character" if new_character else "unknown"),"checks":checks}
print(json.dumps(out,ensure_ascii=False,indent=2))
raise SystemExit(0 if out["ok"] else 2)
