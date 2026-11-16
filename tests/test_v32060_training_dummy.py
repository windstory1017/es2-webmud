from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
DUMMY = ROOT / "source/upstream/mudlib/custom/zhenwu/npc/target_stake.c"

def src(): return DUMMY.read_text(encoding="utf-8")

def test_level_20():
    assert "set_level(20);" in src()

def test_never_retaliates():
    s=src(); assert "void fight_ob(object ob) { }" in s; assert "void kill_ob(object ob) { }" in s

def test_always_hittable():
    s=src(); assert "int defend(int ability, int strength, object from) { return 0; }" in s; assert "int absorb(int ability, int strength, object from) { return 0; }" in s

def test_no_real_damage():
    s=src(); b=s.split("varargs int receive_damage",1)[1].split("void die()",1)[0]; assert "consume_stat" not in b; assert "damage_stat" not in b; assert "return damage;" in b

# 已移除「要有 6 句以上命中訊息」的檢查：訓練木樁被打時不顯示受傷訊息是刻意的（使用者決定，2026-11-16）。
