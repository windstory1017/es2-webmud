# CUSTOM RACE A-H

This is a user-approved WebMUD extension layer. It is not claimed as canonical `taedlar/es2_mudlib` data when the original repository does not provide the race implementation.

Implemented race set:

- human / 人類
- avatar / 人類族
- blackteeth / 黑齒
- yenhold / 厭火
- jiaojao / 焦僥
- woochan / 無腸
- dingling / 釘靈
- headless / 刑天
- rainner / 雨師妾
- malik / 巫首
- yaksa / 夜叉
- ashura / 阿修羅

## Source priority

The 2006-09-15 original race data (`種族詳細資料`, race.detail.txt) takes priority over the earlier `種族設定.html` table and over earlier locked review rules. Class level caps, hidden bonuses, attribute ranges, karma, 基數 and race-skill formulas all follow it. Where the original is silent, the user-approved rules below fill the gap.

## Numbers

- Table gin/kee/sen values are birth values.
- Class level caps are stored as `class_level_cap` in the race daemon and enforced by `feature/char/score.c` at level-up time. `commoner` (平民) is capped at 1 for every race. A cap of `-1` means the original data's 「無」: the race cannot join that class. `set_class()` refuses the change and returns 0.
- 基數 is stored on the race daemon as `commoner_score_base` (percent, 100 = human). The soldier and taoist class daemons multiply their character level-up thresholds by it. It never affects skill experience. Future class daemons must apply it the same way.
- Hidden bonuses follow the original data. 防禦力 = `apply/armor`, 攻擊能力值 = `apply/attack`, 防禦能力值 = `apply/defense`, 攻勢等級 = `apply/intimidate`, 守勢等級 = `apply/wittiness`, 警覺性 = `apply/awarness`. 縱躍閃躲之法 / 拆招卸力之法 are skill levels (`apply/dodge`, `apply/parry`).
- Existing characters keep their rolled attributes. The new ranges apply only to newly created characters. Hidden bonuses are re-applied at every login, so they update for everyone.

## Tick basis

Neolith `HEARTBEAT_INTERVAL` is 2,000,000 microseconds, so 1 tick = one 2-second heartbeat. Conditions are only updated every 5~14 heartbeats, so per-tick race effects (gnaw poison, dance durations, sorrow/rite) are driven by call_out chains in their condition daemons, tied to a per-application serial. ES2 world time advances at 60x real time, so one quarter of an ES2 day is 360 real seconds.

## Race skills

- human `resurge`, avatar `radiate`, jiaojao `hide`: unchanged earlier design. The original's 焦僥 steal/anti-steal advantage is represented by 警覺性 100.
- yenhold `breathe`: every enemy in the room takes kee damage = current kee / 10 + cor x 2. No self damage. Costs sen 10, cooldown 1 tick.
- dingling `hoof`: success roll unchanged (行動力 ratio). On success the target loses kee = dingling str and is busy 2~3 ticks; self busy 1. On failure, self busy 2.
- woochan `replete`: costs sen 20, water maximum / 6 (clamped at zero), busy 1 tick, no cooldown (unchanged). Heals damaged gin/kee/sen bars by age. HP current and maximum +(1 + age/20). Fatigue -(1 + age/20).
- blackteeth `gnaw`: bite and poison rolls unchanged. The bite deals kee 1 + age/20. The poison deals kee 5 + age/15 (max 15) every tick for 1 + age/10 ticks (max 12). All divisions are integer divisions.
- headless `dance for <mode>`: requires a wielded axe and cannot be used in combat.
  - `glory` 榮光之舞: damage +5+str/2, armor +5+cps/2, for int/2 ticks.
  - `fury` 忿怒之舞: damage +15, intimidate +dex/2, for cor/2 ticks.
  - `axe` 斧舞: axe / secondhand axe / twohanded axe skills +int+10, for wis/2 ticks.
  - glory / fury / axe share one slot; a new one replaces the old. Busy 2 after use.
  - `sorrow` 哀傷之舞: each application restores 3~5 of a random one of gin/kee/sen, lowers the other two by 1~2 (never below 1), and costs food and water 1 each.
  - `rite` 祭舞: each application restores gin 2~3, kee 3~5, or sen 1~2 (random).
  - sorrow / rite apply once immediately, then once per tick for wis/2 more ticks, with a dance message each tick. They stop when the dancer enters combat, leaves the room, or stops wielding an axe.

## 雨師妾 (rainner) snakes

- Race numbers follow the original data. Hidden 法力值 10 is the extra term of 法術技巧 (`query_ability("magic")` = spi x int / 10 + sen / 32 + `apply/magic_ability`). `feature/char/combat.c` was changed so this extra term no longer shares `apply/magic` with the 法術 (`magic`) skill level.
- On each level-up a rainner has a 1/2 chance to receive a snake of a colour it does not own yet, up to five snakes.
- Snake experience is stored on the player (`rainner/snake/<colour>`), and worn state in `rainner/worn/<colour>`. The race `setup()` recreates the snake objects at every login and re-wears the ones that were worn, so snakes never disappear on logout.
- Snakes (`custom/race/obj/rainner_snake.c`) weigh 0 and refuse any move away from their owner. They cannot be dropped, given, put, stolen or sold, and they stay with the owner on death. Only the owner can wear them.
- Spots: the largest n with n² x 10 <= accumulated experience, max 120. Bonuses scale linearly as full value x spots / 120.

| Snake | Slot | Full bonus at 120 spots |
|---|---|---|
| 白蛇 white viper | 腰帶 waist | 根骨 5, 防禦力 25, 守勢等級 25, 冰屬性防禦 100 |
| 黑蛇 black viper | 護腿 leg | 機敏 5, 攻勢等級 30, 攻擊能力值 50, 警覺 100 |
| 青蛇 green viper | 頭飾 head | 靈性 5, 咒術技巧 (`apply/spell`) 20, 法術技巧 (`apply/magic_ability`) 20, 風屬性防禦 100 |
| 赤蛇 red viper | 手套 hand | 膂力 5, 傷害力 20, 攻勢等級 30, 火屬性防禦 100 |
| 黃蛇 yellow viper | 項鍊 neck | 定力 5, 行動力 50, 防禦能力值 50, 雷屬性防禦 100 |

- `feed <snake>`: each feed gives 20 + random(spots) experience, busy 1 tick, no cooldown. Refused at 120 spots.

| Snake | Requires | Costs |
|---|---|---|
| white | gin > 1, kee > 10, sen > 10 | gin spots x 2 + 5, kee 10, sen 10 |
| black | gin > spots x 3 + 7, kee > 10, sen > 10 | gin spots x 3 + 7, kee 10, sen 10 |
| green | gin > spots x 2 + 7, kee > 15, sen > 5 | gin spots x 2 + 7, kee 15, sen 5 |
| red | gin > spots x 2 + 8, kee > 10, sen > 5 | gin spots x 2 + 8, kee 10, sen 5 |
| yellow | gin > spots x 2 + 8, kee > 10, sen > 5 | gin spots x 2 + 8, kee 10, sen 5 |

- If gin is below the cost the feed still happens; gin bottoms out and the character falls unconscious.

## 巫首 (malik)

- Race numbers follow the original data: gin/kee/sen 70/40/100, karma 30, 基數 150.
- Class caps: fighter 50, alchemist 50, scholar 60, taoist 60, soldier 50, thief 50, monk 65, commoner 1.
- Attributes: str 8-11, cor 8-11, int 25-30, spi 25-30, cps 23-28, dex 20-25, con 10-15, wis 20-25.
- Hidden bonuses: 防禦力 40 (`apply/armor`), 攻擊能力值 30 (`apply/attack`), 防禦能力值 30 (`apply/defense`).
- No race skill. The internal id `malik` matches the existing `who -ml` filter and the `data/chinese.o` entry.

## 夜叉 (yaksa)

- Race numbers follow the original data: gin/kee/sen 140/70/70, karma 50, 基數 140.
- Class caps: fighter 65, alchemist 50, taoist 50, soldier 50, thief 70, commoner 1. scholar and monk are -1 (cannot join).
- Attributes: str 16-25, cor 17-25, int 13-18, spi 13-18, cps 13-18, dex 13-18, con 16-25, wis 13-18.
- Hidden bonuses: 防禦力 15 (`apply/armor`), 攻擊能力值 10 (`apply/attack`), 陰陽眼 (`apply/vision_of_ghost`).
- `devour <ghost>` (`daemon/race/yaksa/devour.c`): targets anything whose `life_form` is `ghost` (ghost NPCs and dead players). Not in combat, not in no_fight rooms, not on wizards.
  - One bite per tick. Bite = cor + dex + random(own current gin) / 10, taken from the ghost's current gin. At 0 the ghost is devoured. The yaksa and a ghost NPC are busy while devouring. A player ghost can still move and escapes by leaving the room. It stops if the yaksa enters combat, dies, or either side leaves the room.
  - Ghost NPC: destroyed. The yaksa heals gin and sen (current and damaged maximum) by the total amount devoured.
  - Player ghost: 魂飛魄散 through the existing `CHAR_D->make_mist()` reincarnation. The account (id, password, total online time `time_aged`) is kept, the character file is deleted, and the player picks a race again.
  - Player ghost reward (no backlash, even when the ghost is stronger): "ghost gin" G = the ghost's current gin when devouring started, doubled for a yaksa ghost. The yaksa permanently gains maximum gin 1 + G/40, kee 1 + random(G/40), sen 1 + random(G/80). The first 10 player devours always give this large gain. After that the chance is 10 / (10 + random(count)); otherwise the gain is gin 1~2, kee 0~1. The count is stored in `yaksa/devoured_players`.

## 阿修羅 (ashura)

- Race numbers follow the original data: gin/kee/sen 80/80/80, karma 40, 基數 180.
- Class caps: fighter 70 (raised from 60 by user, 2026-10-20), alchemist 50, taoist 60, soldier 70, thief 50, commoner 1. scholar and monk are -1 (cannot join).
- Attributes: str 20-25, cor 25-40, int 13-18, spi 14-22, cps 5-10, dex 15-20, con 17-22, wis 13-18.
- Hidden bonuses: 攻擊能力值 40 (`apply/attack`), 攻勢等級 30 (`apply/intimidate`), 咒術 15 (`apply/spells`, skill level), 陰陽眼 (`apply/vision_of_ghost`).
- Auto fight: when an ashura player and an NPC first meet (either one walks into the room), `feature/char/attack.c` `init()` calls `COMBAT_D->start_ashura()`. Chance = 100 - cps x 3, minimum 30%. It uses `fight`, not `kill`, on both sides, so the loser only falls unconscious and the player can `halt`. Skipped when the ashura is already fighting, unconscious, net-dead or a ghost, when the NPC is unconscious, a ghost or not visible, and in no_fight rooms. Players are never targeted. It cannot be turned off.

## Character creation limit

`adm/daemons/logind.c` (`ENABLE_ANTISPAM`): new characters no longer lose attribute points. Each IP may create at most 10 characters within 30 minutes of its first creation. The 11th new ID from that IP inside the window is refused at the "create this ID?" step. Existing characters can always log in. Reincarnation (re-creating after 魂飛魄散) counts toward the 10 but is not blocked.

## Class level-up

Every class level-up threshold is multiplied by the race 基數 (`commoner_score_base` / 100). A threshold `(lv-N)` or `(lv-N)^2` is 0 until the level passes N. `lv` is the level being reached. Shared helpers live in `include/class_level.h`. Player thresholds are recomputed at every login (`CHAR_D->setup_char()`), so formula changes apply to existing characters.

On level-up, gin / kee / sen maximums grow by base dex / con / spi divided by the class divisor, each +1, +0 or -1 at random, never below 0.

| Class | Thresholds (x 基數) | gin / kee / sen growth |
|---|---|---|
| 武者 fighter | 實戰經驗 combat (lv-1)^2x150, 武術造詣 martial art (lv-1)^2x150, 武學之道 martial mastery (lv-10)^2x100 | dex/4, con/2, spi/8 |
| 盜賊 thief | 江湖歷練 survive (lv-1)x100, 實戰經驗 combat (lv-1)^2x100, 偷盜伎倆 thievery (lv-1)^2x100, 黑道聲望 negative fame (lv-31)x100 | dex/2, con/4, spi/8 |
| 方士 alchemist | 江湖歷練 survive (lv-1)^2x100, 法術道行 magic mastery (lv-16)^2x100, 丹道修養 alchemy (lv-1)^2x100, 法術修為 magic (lv-6)^2x100 | dex/4, con/4, spi/4 |
| 書生 scholar | 江湖歷練 survive (lv-1)^2x100, 實戰經驗 combat (lv-1)^2x100, 文書能力 literature (lv-1)^2x50, 文學造詣 literature mastery (lv-10)^2x50, 聲望 reputation (lv-10)^2x50 | dex/5, con/3, spi/3 |
| 和尚 monk | 江湖歷練 survive (lv-1)^2x100, 佛學修為 buddhology (lv-1)^2x100, 禪定修養 cultivation (lv-10)^2x100, 文書能力 literature (lv-10)^2x100, 聲望 reputation (lv-30)^2x100 | dex/6, con/6, spi/6 |
| 平民 commoner | 江湖歷練 survive (lv-1)^2x100 | none (user decision, 2026-10-20) |

Commoner (user decision, 2026-10-20): may level up on 江湖歷練 alone; the commoner level cap is 50 for every race (`feature/char/score.c` `at_level_cap()`), overriding the `"commoner":1` entries still listed in the race daemons. A commoner who later joins a class keeps the current level.

Soldier and taoist keep their earlier formulas. Joining thief / alchemist / scholar / monk is not implemented yet.

### Combat score gains (NEW)

`adm/daemons/combatd.c`, players only:

- Successful dodge (`restored_dodge_gain`) or parry (`restored_parry_gain`) gives 實戰經驗 `combat`. Against a player: `random(2)` (0 or 1). Against an NPC whose level is not above yours: 1. NPC n levels above you: `1 + random(n)`, n capped at 10. An NPC without a level counts as level 0.
- A hit that deals damage (`restored_hit_gain`) with an enabled martial art gives 1 武術造詣 `martial art`. Hits made with a basic attack skill (unarmed, beast, and every blade / sword / axe / pike / staff / blunt / dagger / needle / whip form) give none.
- `feature/char/skill.c` `advance_skill()`: when a basic skill reaches level sk it gives sk x 10 武術造詣 (every level counts when several are gained at once). Basic skills: unarmed, dodge, parry, and every blade / sword / axe / pike / staff / blunt / dagger / needle / whip form.

## Ghost NPCs (for yaksa `devour`)

`custom/ghost/std_ghost.c` is the base: human race, `life_form` ghost (no kee, only visible with 陰陽眼, ordinary attacks pass through), peaceful, refuses `fight`. Talking and moving work like 瞎眼老太婆 (`d/snow/npc/gammer.c`): `chat_chance` 9 gives a 10% chance each tick to act, then one of three `chat_msg` entries is picked (one says one of its two lines, two do `random_move`). On average it talks every ~30 ticks and moves every ~15 ticks, at random times. It does neither while busy (for example while being devoured).

Color rule: every ghost is shown in dark gray (`HIK`, the same color as a dead player's ghost). The base colors the ghost's name (`name()`, so movement and `$N` action messages), its room short description and its chat lines. All future ghost NPCs must inherit `custom/ghost/std_ghost.c` so they follow this rule.

| NPC | File | id | gin / sen |
|---|---|---|---|
| 餓鬼 | `custom/ghost/npc/hungry_ghost.c` | hungry ghost | 20 / 20 |
| 遊魂 | `custom/ghost/npc/wandering_soul.c` | wandering soul | 40 / 20 |
| 枉死鬼 | `custom/ghost/npc/wronged_ghost.c` | wronged ghost | 50 / 25 |
| 倀鬼 | `custom/ghost/npc/tiger_thrall.c` | tiger thrall | 60 / 30 |
| 魑魅 | `custom/ghost/npc/mountain_sprite.c` | mountain sprite | 100 / 20 |
| 魍魎 | `custom/ghost/npc/water_goblin.c` | water goblin | 100 / 20 |

All of them also answer to `ghost`.

Placement (room `objects`, 雪亭鎮): 餓鬼 x2 in `/d/snow/square`; 遊魂 x1 each in `/d/snow/nstreet1` and `/d/snow/ruin1`; 枉死鬼 in `/d/snow/ebridge`; 倀鬼 in `/d/snow/ngate`; 魑魅 in `/d/snow/wgate`; 魍魎 in `/d/snow/river1`. A room only replaces a ghost it spawned once that ghost is destroyed (for example devoured), at the room's next reset, so wandering ghosts are never duplicated.
