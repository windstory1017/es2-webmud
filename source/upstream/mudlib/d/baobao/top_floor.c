// /d/baobao/top_floor.c — 八寶樓（D2）頂樓。敘述：舊資料（刪「上」「所」兩字）。設計見 docs/AREA_BAOBAO.md。
// 沒有一般出口，只能從敞開的窗戶跳出去，回到將軍府前廣場。

#include <room.h>

inherit ROOM;

void init()
{
    ::init();
    add_action("do_jump", "jump");
}

// 跳窗：落地站穩的機率 = 閃躲技能 / 300（技能最高算 200，所以最強的人仍有 1/3 機會摔）。
int do_jump(string arg)
{
    object me = this_player();
    int max;

    if( arg != "窗戶" ) return 0;
    if( me->is_busy() || me->is_fighting() )
        return notify_fail("你現在沒有辦法這麼做。\n");

    message_vision("$N往窗戶一躍，縱身而下！\n", me);
    if( !me->move("/d/fengcheng/general_gate") ) return 0;
    message("vision", me->name() + "從高處躍了下來。\n", environment(me), ({ me }));

    if( random(300) >= me->query_skill("dodge") ) {
        message_vision("$N落地時沒有站穩，重重摔在地上，眼前一黑，昏了過去。\n", me);
        if( max = me->query_stat_maximum("kee") )
            me->damage_stat("kee", max / 10);
        me->unconcious();
    }
    return 1;
}

void create()
{
    set("short", "頂樓");
    set("long", @LONG
頂樓佈置的宛如皇宮一般，金碧輝煌的屋頂有著工匠精心做的龍形雕刻。環繞四周的牆壁上面有著七幅畫像，據說各代表著這裡主人的七個兄弟，只見七人的臉部表情都栩栩如生，彷彿真的有七個風度翩翩的君子站在這裡一樣。此外，周圍的擺設更是盡奢侈之能事，你不禁懷疑究竟什麼人有資格住在這間屋子裏。頂樓的南面有一扇敞開的窗戶，也許可以跳出去。
LONG
    );
    set("detail/窗戶", "一面敞開的窗戶，縱身一躍 (jump) 就可以回到將軍府前廣場。\n");
    set("objects", ([
        "/custom/legacy_npc/xun_yuan_jian_yue" : 1,
    ]));
    set("map/area", "八寶樓");
    set("map/mode", "maze");
    setup();
}
