# 冷梅莊莊主 梅影風（may yin fong）

> CUSTOM NPC（NEW）。設定由使用者提供（2026-10-09）。檔案：`source/upstream/mudlib/d/wutang/npc/may_yin_fong.c`。

## 基本

- 人類、武者、LV60、強度 S、55 歲，稱號「冷梅莊莊主」。
- 精氣神由 NPC 強度系統計算，約 精 800／氣 1750／神 600（依種族屬性擲骰浮動）。
- 裝備：銀霜鎧、奔雲靴、梅花白襖、白玉腰帶、白玉戒指、寒霜劍（右手）、冰雪匕（左手）。
  白玉腰帶世上只有一條，已在別人手上時改穿銀彎束腰。

## 武功

| 技能 | 等級 | 使用 |
|---|---|---|
| 冷梅劍法 lunmay | 180 | 劍術、招架 |
| 傲梅暗劍訣 advance_lunmay | 180 | 左手劍術、左手錐法 |
| 寒梅心法 hainmay force | 200 | 內功 |
| 梅影身法 mayin | 160 | 閃躲 |
| 七巧如意手 seven | 160 | 徒手 |
| sword / secondhand sword / secondhand dagger | 180 | |
| force | 190 | |
| parry / dodge / unarmed | 160 / 150 / 140 | |

## 所在（2026-10-30 起）

- 在冷梅莊正廳 `/d/swaylan/lunmay/hall` 出生，**不走動**（使用者指示，2026-10-30）。
- 檔案仍放在 `d/wutang/npc/may_yin_fong.c`。過去在五堂鎮交叉路口出生、在五堂鎮內走動，已取消。

## 拜師（apprentice may）

1. 不是平民：「閣下的志向相當明確，又何必來糾纏老朽呢？」
2. 阿修羅、巫首、雨師妾：「閣下似乎不適合修習本門武功。」
3. 通過：成為武者，`custom_faction` = `fighter.lunmay`，稱號「冷梅莊弟子」。

## 傳授（acquire <技能> from may）

只教冷梅莊弟子；每門只補足到剛好可以升到 1 級的經驗，之後要自己修練。

| 條件 | 技能 |
|---|---|
| 拜師後 | 梅影身法、七巧如意手、sword、secondhand sword、secondhand dagger、parry、dodge、unarmed |
| LV15 | 寒梅心法、force |
| LV30 且寒梅心法 100 | 傲梅暗劍訣 |

**冷梅劍法不傳授**（使用者指示，2026-10-30）：求教時梅影風說
「冷梅劍法的要訣需要自行體會，等你能夠一劍七環時就算有所小成了。」
冷梅莊弟子要到冷梅莊前院練穿環學成，見 `docs/AREA_SWAYLAN.md` 第二批。
