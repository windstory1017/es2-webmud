from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
MUD = ROOT / "source/upstream/mudlib"
BASE = MUD / "feature/npc/dummy.c"
DUMMIES = [
    MUD / "custom/zhenwu/npc/target_stake.c",
    MUD / "d/swaylan/lunmay/npc/stake.c",
    MUD / "d/baihua/npc/stone_lion.c",
]

def base(): return BASE.read_text(encoding="utf-8")

def test_level_20():
    for p in DUMMIES:
        assert "set_level(20);" in p.read_text(encoding="utf-8"), p

def test_all_dummies_use_shared_base():
    # 所有假人類 NPC（木樁、石獅子）都繼承共用基底，不各自複製一份覆寫。
    for p in DUMMIES:
        s = p.read_text(encoding="utf-8")
        assert 'inherit "/feature/npc/dummy";' in s, p
        assert "inherit F_VILLAGER" not in s, p
        assert "void die()" not in s, p

def test_no_other_training_dummy_npc_outside_base():
    # 任何設了 training_dummy 的 NPC 檔，都必須是共用基底或繼承它的檔案。
    for p in MUD.rglob("*.c"):
        s = p.read_text(encoding="utf-8", errors="ignore")
        if p != BASE and 'set("training_dummy", 1)' in s and "/npc/" in p.as_posix():
            assert 'inherit "/feature/npc/dummy";' in s, p

def test_never_retaliates():
    s = base(); assert "void fight_ob(object ob) { }" in s; assert "void kill_ob(object ob) { }" in s

def test_always_hittable():
    s = base()
    assert "int defend(int ability, int strength, object from) { return 0; }" in s
    assert "int absorb(int ability, int strength, object from) { return 0; }" in s

def test_no_real_damage():
    s = base(); b = s.split("varargs int receive_damage", 1)[1].split("/* 無生命體", 1)[0]
    assert "consume_stat" not in b and "damage_stat" not in b and "return damage;" in b

def test_no_life_phenomena():
    s = base()
    for sig in ("varargs void unconcious(string reason) { }", "varargs void revive(int quiet) { }",
                "void die() { }", "static void heart_beat() { set_heart_beat(0); }",
                "varargs int consume_stat(string type, int damage, object who) { return 0; }",
                "varargs int damage_stat(string type, int damage, object who) { return 0; }"):
        assert sig in s, sig
    assert "set_heart_beat(0);" in s.split("void setup()", 1)[1].split("}", 1)[0]

def test_dummy_immune_to_conditions():
    s = (MUD / "feature/char/condition.c").read_text(encoding="utf-8")
    b = s.split("set_condition(string cnd, mixed info)", 1)[1].split("conditions[cnd] = info", 1)[0]
    assert 'this_object()->query("training_dummy")' in b and "return;" in b
