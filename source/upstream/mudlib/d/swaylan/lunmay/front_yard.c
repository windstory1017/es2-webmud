// /d/swaylan/lunmay/front_yard.c — 冷梅莊（I25）。房間敘述照設計表原文（舊站原文）。

#include <room.h>

inherit ROOM;

#define RINGS_MARK  "lunmay/rings"   // 累積穿環次數（存在角色身上）
#define RINGS_DONE  36                // 累積 36 次一劍穿過七個環，學成冷梅劍法
#define GIN_COST    10

void init()
{
    ::init();
    add_action("do_practice", "practice");
}

/* 穿環練冷梅劍法（使用者指示，2026-10-30；2026-10-31 指令改為 practice、自己看到的訊息依性別顯示你／妳）：冷梅劍法不再由梅影風傳授，
 * 冷梅莊弟子要在這裡自行練習。每次耗精 10、忙碌 1 回合；
 * 環數＝1＋累積次數開根號（設計表：練習 n² 次可多刺中一個環），累積 36 次七個環即學成。 */
int do_practice(string arg)
{
    object me = this_player();
    int count, rings;

    if( arg != "lunmay" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail(gender_self(me) + "現在沒有辦法這麼做。\n");
    if( me->query("custom_faction") != "fighter.lunmay" )
        return notify_fail(gender_self(me) + "不是冷梅莊弟子，這些環不是給" + gender_self(me) + "練的。\n");
    if( me->query_skill("lunmay", 1) > 0 )
        return notify_fail(gender_self(me) + "已經練成冷梅劍法，不用再在這裡練習了。\n");
    if( !objectp(me->query_temp("weapon/sword")) )
        return notify_fail(gender_self(me) + "手中沒有劍，要拿什麼穿環？\n");
    if( me->query_stat("gin") < GIN_COST )
        return notify_fail(gender_self(me) + "精神不濟，再練下去只怕連一個環也穿不過。\n");

    me->consume_stat("gin", GIN_COST);
    me->start_busy(1);
    count = me->query(RINGS_MARK) + 1;
    me->set(RINGS_MARK, count);
    rings = 1 + to_int(sqrt(to_float(count)));
    if( rings > 7 ) rings = 7;

    if( count < RINGS_DONE ) {
        write(gender_self(me) + "調整氣息、聚精凝神一劍使出，結果穿過了" + chinese_number(rings) + "個環。\n");
        tell_room(this_object(), me->name() + "調整氣息、聚精凝神一劍使出，結果穿過了"
            + chinese_number(rings) + "個環。\n", ({ me }));
        return 1;
    }

    write(gender_self(me) + "調整氣息、聚精凝神，一劍穿過了七個環的中心，劍尖竟未碰到半點環壁！\n");
    tell_room(this_object(), me->name() + "調整氣息、聚精凝神，一劍穿過了七個環的中心，"
        + "劍尖竟未碰到半點環壁！\n", ({ me }));
    // 直接學成：補足到學成那一級所需的點數並當場升級（冷梅劍法顯示「你終於練成了冷梅劍法。」）。
    me->set_learn("lunmay", me->skill_threshold("lunmay", me->skill_entry_level("lunmay")));
    me->apply_gain_progression(([ "lunmay" : 1 ]));
    me->delete(RINGS_MARK);
    return 1;
}

void create()
{
    set("short", "冷梅莊前院");
    set("long", @LONG
這裡是冷梅莊練劍的地方, 在院子裡有一些木製的橫樑, 在上面掛了很多空心的環, 高低不一, 聽說冷梅劍練到一定的火侯, 能夠一劍穿過所有環的中心而不碰觸到環, 院子裡還有一些椅子, 供練劍的人累了休息用.
LONG
    );
    set("detail", ([
        "環" : "在前院的橫樑中，懸掛著七個空心的鐵環，讓弟子可以練習 (practice) 冷梅劍法。",
    ]));
    set("exits", ([
        "north" : __DIR__"courtyard",
        "south" : __DIR__"path",
        "east" : __DIR__"drill_s",
    ]));
    set("objects", ([
        "/custom/legacy_npc/may_lan_wu" : 1,
    ]));
    set("map/area", "冷梅莊");
    setup();
}
