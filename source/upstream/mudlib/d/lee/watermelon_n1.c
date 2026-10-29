// /d/lee/watermelon_n1.c
// 李家村復原版。RECONSTRUCTED
// Topology follows the user-provided Li Village map; generated prose is intentionally replaceable.

inherit ROOM;

#define MAX_PICK 3

int picked;   // 這一輪已經被摘走幾顆

void create()
{
    set("short", "西瓜田");
    set("long", @LONG
田畦裡種著一排排西瓜藤，寬大的葉片鋪滿泥地。農人沿著田埂巡看瓜苗，防著路人隨手摘走尚未成熟的西瓜。幾顆碩大的西瓜半埋在泥土裡，表皮泛著油亮的青綠色，看著十分誘人。田埂邊插著一根根竹竿，上頭綁著幾條破布條隨風擺動，是農人用來嚇唬偷嘴的鳥雀。
LONG
    );
    set("objects", ([
        "/d/lee/npc/farmer" : 3
    ]));
    set("exits", ([
        "east" : __DIR__"watermelon_n2",
        "south" : __DIR__"watermelon_s1"
    ]));
    set("detail/西瓜", "圓滾滾的大西瓜，或許可以趁農夫不注意的時候摘（pick）幾個？");
    set("detail/watermelon", "圓滾滾的大西瓜，或許可以趁農夫不注意的時候摘（pick）幾個？");
    set("map/area", "李家村");
    setup();
}

void reset()
{
    picked = 0;
    ::reset();
}

void init()
{
    ::init();
    add_action("do_pick", "pick");
}

int do_pick(string arg)
{
    object melon, farmer;
    if( !arg || (arg != "西瓜" && arg != "watermelon" && arg != "melon") ) return 0;
    // 這片瓜田每次重生（reset）只有三顆成熟的西瓜可摘，所有人共用（使用者定案，2026-10-29）。
    if( picked >= MAX_PICK ) {
        write("田裡成熟的西瓜都被摘光了，剩下的全是青澀的小瓜。\n");
        return 1;
    }
    picked++;
    melon = new("/d/lee/obj/watermelon");
    melon->move(this_player());
    message_vision("$N彎下腰，從瓜田裡摘起一顆西瓜。\n", this_player());
    foreach(farmer in all_inventory(this_object())) {
        if( farmer->id("lee farmer") ) {
            tell_room(this_object(), farmer->name() + "怒道：誰准你亂摘田裡的西瓜！\n");
            farmer->kill_ob(this_player());
        }
    }
    return 1;
}
