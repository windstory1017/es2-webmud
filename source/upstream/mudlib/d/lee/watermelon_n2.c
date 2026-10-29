// /d/lee/watermelon_n2.c
// 李家村復原版。GENERATED
// Topology follows the user-provided Li Village map; generated prose is intentionally replaceable.

inherit ROOM;

#define MAX_PICK 3

int picked;   // 這一輪已經被摘走幾顆

void create()
{
    set("short", "西瓜田");
    set("long", @LONG
大片瓜藤沿田埂向東展開，葉下偶爾露出圓滾滾的西瓜。日照充足時，田間熱氣與泥土氣味格外明顯。幾名農人戴著斗笠，彎腰仔細查看瓜藤的長勢，時不時直起身子擦一把汗，對這片瓜田的收成顯然十分上心。
LONG
    );
    set("exits", ([
        "west" : __DIR__"watermelon_n1",
        "east" : __DIR__"watermelon_n3",
        "south" : __DIR__"watermelon_s2"
    ]));
    set("objects", ([
        "/d/lee/npc/farmer" : 3
    ]));
    set("detail/西瓜", "圓滾滾的大西瓜，或許可以趁農夫不注意的時候摘（pick）幾個？");
    set("detail/watermelon", "圓滾滾的大西瓜，或許可以趁農夫不注意的時候摘（pick）幾個？");
    set("map/area", "李家村");
    setup();
    // MODIFIED: no replace_program(ROOM) here — this file defines its own
    // init()/do_pick(); replacing the program to plain ROOM after create()
    // would silently discard them, so "pick" would stop working. (Same bug
    // found and fixed in hunter.c / d/lee/pawnshop.c today.)
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
