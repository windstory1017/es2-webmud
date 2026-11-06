# 門派說明檔與門派代碼計畫

建立日期：2026-11-06（使用者指示 #35）。依據：使用者提供的原版 ES2 門派清單截圖。

## 原則

- 門派說明檔（`help 職業.門派`）與遊戲內部的門派代碼（`custom_faction`）一律使用原版清單的舊名。
- 2026-11-06 起，下列七個代碼已改為舊名；存檔裡還是舊代碼的玩家，登入時自動換成新代碼（`adm/daemons/logind.c`）：

| 門派 | 改前 | 改後 |
|---|---|---|
| 武陀灸堂 | `alchemist.wuto` | `alchemist.wu` |
| 幻霧觀 | `taoist.fog` | `mao-shan.fog` |
| 隱風觀 | `taoist.wind` | `mao-shan.wind` |
| 靈雲觀 | `taoist.lingyun` | `mao-shan.cloud` |
| 哭笑門 | `fighter.chin` | `fighter.huge` |
| 古劍門 | `fighter.ancient` | `fighter.au-chan` |
| 隱教 | `thief.hidden` | `thief.yin` |

- 說明檔格式照 `CLAUDE.md`「說明檔格式」（以 `fighter.tiger.txt` 為準）；做好後加進 `help topics` 的門派區。

## 門派清單與狀態

狀態：**已完成**＝有說明檔；**待補說明**＝門派已在遊戲裡，還沒有說明檔；**未製作**＝遊戲裡還沒有這個門派。

### 武者（fighter）　共通指令：taunt

| 門派 | 代碼 | 狀態 |
|---|---|---|
| 封山派 | `fighter.fonxan` | 已完成 |
| 雪吟莊 | `fighter.snow` | 待補說明 |
| 冷梅莊 | `fighter.lunmay` | 已完成 |
| 古劍門 | `fighter.au-chan` | 待補說明 |
| 虎刀門 | `fighter.tiger` | 已完成 |
| 哭笑門 | `fighter.huge` | 待補說明（青邪杖法 `chin_staff` 尚無技能檔） |
| 青邪禁衛 | `fighter.7b` | 未製作 |

### 盜賊（thief）　共通指令：peek、concentrate

| 門派 | 代碼 | 狀態 |
|---|---|---|
| 瑯夷派 | `thief.longye` | 待補說明 |
| 隱教 | `thief.yin` | 待補說明 |
| 黑龍使者（進階） | `thief.adv` | 未製作 |

### 方士（alchemist）　共通指令：assess、acupuncture

| 門派 | 代碼 | 狀態 |
|---|---|---|
| 武陀灸堂 | `alchemist.wu` | 已完成 |
| 龍圖丹派 | `alchemist.dragon` | 已完成 |
| 劍甲門（進階） | `alchemist.adv` | 未製作 |

### 軍人（soldier）

| 門派 | 代碼 | 狀態 |
|---|---|---|
| 振武軍營 | `soldier.ber` | 已完成 |

### 書生（scholar）　共通指令：organize、review

| 門派 | 代碼 | 狀態 |
|---|---|---|
| 玄天教 | `scholar.sky` | 待補說明 |
| 步玄派 | `scholar.myst` | 已完成 |

### 道士（taoist）　共通指令：hearth

| 門派 | 代碼 | 狀態 |
|---|---|---|
| 朱衣派 | `taoist.fire` | 已完成 |
| 玄衣派 | `taoist.storm` | 已完成 |
| 素衣派 | `taoist.freeze` | 待補說明 |
| 紫衣派 | `taoist.thunder` | 待補說明 |
| 幻霧觀 | `mao-shan.fog` | 已完成 |
| 隱風觀 | `mao-shan.wind` | 已完成 |
| 靈雲觀 | `mao-shan.cloud` | 待補說明 |
| 道士進階 | `taoist.adv` | 未製作 |

### 僧侶（bonze）　共通指令：zazen、meditate、paramita

| 門派 | 代碼 | 狀態 |
|---|---|---|
| 天月庵 | `bonze.moon` | 未製作 |
| 白象寺 | `bonze.wele` | 未製作 |
| 寶蓮寺 | `bonze.kind` | 未製作 |

## 職業共通指令說明

原版清單在每個職業下列了共通指令（上表標題）。目前沒有職業層級的說明檔，這些指令的說明之後另行規劃。

## 進行方式

- 每補一份說明檔，把上表狀態改成「已完成」並註記日期。
- 「未製作」的門派要先有門派設計（`docs/martial_arts/`），經使用者確認後才製作。
