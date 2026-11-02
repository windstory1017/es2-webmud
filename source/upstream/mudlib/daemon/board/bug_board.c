/*---
description: 提供玩家回報問題的留言板。
author: Annihilator <taedlar@gmail.com>
---*/
inherit BULLETIN_BOARD;

private void
create()
{
  set_name("問題回報與建議留言板", ({"bug board", "board"}));
  set("location", "/d/snow/bug_room");
  set("board_id", "bugs");
  set("capacity", 100);
  setup();
  set("long", "這是一個提供玩家回報問題、提出建議的留言板。\n");
  replace_program(BULLETIN_BOARD);
}

