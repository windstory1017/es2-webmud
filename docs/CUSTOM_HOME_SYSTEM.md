# Custom Home System

> This is a WebMUD custom extension and is **not** canonical `taedlar/es2_mudlib` content.

## 入口現況（使用者決定，2026-10-23）

- 家園傳送師已移除；`recall`、`recall home` 指令也已取消（含說明檔）。
- **未來不會再開放 recall／recall home 進入家園。** 家園的新入口另行設計。
- 目前玩家沒有進入家園的方式；家園的儲物箱、房間發展等功能仍保留。
- 下方 Phase 1 起的內容是當初的設計紀錄，其中「家園傳送師」「home 指令」已不適用。

## Phase 1

- Snow Inn (`/d/snow/inn_hall`) contains `家園傳送師` (`home teleporter`).
- `home` teleports a player into a freshly-instanced private `家園大廳`.
- Only the owning player may enter that hall instance.
- The hall contains:
  - `私人儲物箱`: 200 object-stack slots, no weight limit because stored contents are serialized into player save data instead of physically carried.
  - `家園管家`: develops up to 10 future rooms.
- Room development prices: 10/20/.../100 gold for rooms 1..10.
- Developed room count is saved in `custom_home/rooms` on the player.
- Storage is saved in `custom_home/storage` on the player. Common item identity, stack amount and autoload string are preserved.
- Future room purposes are intentionally unspecified until the user supplies them.

## Snow Inn custom easter egg

- `三屆打手槍冠軍［鐵手］林楓炎` is a custom NPC in the inn.
- Title, nickname and personal name use ANSI colors.
- Looking at him makes him say: `小子，要來場酣暢淋漓的打手槍比賽嗎？`
- Each developed room is immediately enterable with `enter <1-10>` and uses a generic private room shell until a future purpose is assigned.
