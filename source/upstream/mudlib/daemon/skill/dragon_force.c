/*---
description: 龍圖心經（dragon force）的載入入口。
note: 技能程式本體在 /d/snow/skill/dragon_force.c。系統找技能時，若登記表裡沒有
      「dragon force」，會改找 /daemon/skill/dragon_force；伺服器剛開機時本體還沒
      載入、也沒登記，enable 會出現「不能和其他技能合用」。本檔繼承本體，讓系統
      隨時找得到（2026-10-23 單機測試發現）。
---*/
inherit "/d/snow/skill/dragon_force";
