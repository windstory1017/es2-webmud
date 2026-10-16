inherit COMBINED_ITEM;
void create() {
    set_name("清腹丹", ({"lavage pill", "pill"}));
    if (!clonep()) {
        set("long", "清腹丹是一種藥性峻烈的方家丹藥，由大黃、芒硝、枳實及厚朴等數種藥材煉製而成，具有通腑導滯、蕩除積穢之效。其藥力猛烈，服用後往往腹中翻攪、腸鳴不止，令人便意難抑，若非必要，實不宜輕易服用。\n");
        set("unit", "些"); set("base_unit", "粒");
        set("base_value", 1000); set("base_weight", 5); set("mixture_value", 200);
        set("produce_param", (["min_heat_power":130,"max_heat_power":150,"min_heat":200,"max_heat":400]));
    }
    set("requirement", (["literate": 5, "alchemy-medication": 5]));
    set_amount(1); setup();
}
int stuff_ob(object me) {
    if (me->query_stat_maximum("food") - me->query_stat("food") < 20)
        return notify_fail(gender_self(me) + "的肚子已經撐得連這粒藥丸也吞不下去了。\n");
    message_vision("$N吞下一粒清腹丹。\n", me);
    me->consume_stat("water",10); me->consume_stat("food",30);
    add_amount(-1); return 1;
}
string query_heat_product() { return "lavage"; }
varargs int move(mixed dest, int silent) {
    object ob;
    if (!::move(dest,silent)) return 0;
    foreach(ob in all_inventory(environment())) if(ob!=this_object() && base_name(ob)==base_name(this_object())) {
        add_amount(ob->query_amount()); destruct(ob);
    }
    return 1;
}
