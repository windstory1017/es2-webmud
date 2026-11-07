// /d/wutang/backyard.c — 五堂鎮（Z8）。房間敘述照設計表原文。

#include <room.h>
#include "/d/wutang/butterfly_quest.h"

inherit ROOM;

void init()
{
    ::init();
    add_action("do_search", "search");
}

// 蝶影幻步任務：李山說金牌藏在小溪裡，搜小溪找到金牌（每個玩家各自進行）。
int do_search(string arg)
{
    object me = this_player();

    if( arg != "小溪" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");
    if( (bq_stage(me) != BQ_LEE && bq_stage(me) != BQ_PLAQUE) || bq_find(me, BQ_PLAQUE_FILE) ) {
        write("你在小溪中摸了一陣﹐只摸到幾顆滑溜溜的石子。\n");
        return 1;
    }
    // 進度是 BQ_PLAQUE 卻沒有金牌：金牌弄丟了（賣掉、丟了），讓玩家再找一次。
    me->set(BQ_STAGE, BQ_PLAQUE);
    bq_give(me, BQ_PLAQUE_FILE);
    write("你在小溪中摸索，找到一面金牌。\n");
    message("vision", me->name() + "在小溪中摸索了一陣。\n", environment(me), me);
    return 1;
}

void create()
{
    set("short", "後院");
    set("long", @LONG
這裡是民房的後院，在後院裡有養著各式各樣的家畜，而在不遠的前方也有著一條很清澈的小溪，通常這也是這地方人家洗衣服和用水的小溪，而在小溪中也有幾隻小魚，溪中也有鴨子在上面玩水。
LONG
    );
    set("exits", ([
        "north" : __DIR__"path_e",
    ]));
    set("map/area", "五堂鎮");
    setup();
}
