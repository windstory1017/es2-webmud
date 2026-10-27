from pathlib import Path
import json,sys
ROOT=Path(__file__).resolve().parents[1]; M=ROOT/'source/upstream/mudlib'
files={
 'butler':M/'custom/home/npc/butler.c',
 'chest':M/'custom/home/obj/storage_chest.c','board':M/'custom/home/obj/development_board.c',
 'bed':M/'custom/home/obj/bed.c','hall':M/'custom/home/room/home_hall.c',
 'room':M/'custom/home/room/future_room.c','bedroom':M/'custom/home/room/bedroom.c',
 'studyroom':M/'custom/home/room/study_room.c','studycore':M/'feature/study.c',
 'inn':M/'d/snow/inn_hall.c'}
fail=[]
for k,p in files.items():
    if not p.exists(): fail.append(f'missing:{k}')
text={k:p.read_text(encoding='utf-8') for k,p in files.items() if p.exists()}
checks={
 # 家園傳送師與林楓炎已依使用者要求移除；recall／recall home 也已取消（2026-10-23），家園入口另行設計
 'recall_removed': not (M/'cmds/usr/recall.c').exists(),
 'private_hall':'ob != home_owner' in text.get('hall',''),
 'chest_200':'sizeof(items) >= 200' in text.get('chest',''),
 'storage_saved':'custom_home/storage' in text.get('chest','') and 'me->save()' in text.get('chest',''),
 'board_in_hall':'development_board' in text.get('hall','') and 'look board' in text.get('hall',''),
 'numbered_menu':'1. 寢室' in text.get('board','') and '2. 書房' in text.get('board','') and '5. 尚未開放' in text.get('board',''),
 'dynamic_price':'本次將增建第 %d 間，費用：%d GOLD' in text.get('board','') and 'n * 10' in text.get('board',''),
 'select_actions':'add_action("do_select", "select")' in text.get('board','') and 'add_action("do_select", "選擇")' in text.get('board',''),
 'bedroom_choice':'choice == 1' in text.get('board','') and 'type = "bedroom"' in text.get('board',''),
 'studyroom_choice':'choice == 1' in text.get('board','') and 'type = "study_room"' in text.get('board',''),
 'rooms_saved':'custom_home/rooms' in text.get('board','') and 'me->save()' in text.get('board',''),
 'room_type_saved':'custom_home/room_type/%d' in text.get('board',''),
 'ten_room_cap':'rooms >= 10' in text.get('board',''),
 'gold_formula':'n * 10' in text.get('board','') and 'gold_cost * 10000' in text.get('board',''),
 'bedroom_entry':'room_type/%d' in text.get('hall','') and '/custom/home/room/bedroom' in text.get('hall',''),
 'studyroom_entry':'/custom/home/room/study_room' in text.get('hall',''),
 'studyroom_bonus':'custom_home/study_bonus' in text.get('studyroom','') and '120' in text.get('studyroom',''),
 'studycore_bonus':'custom_home/study_bonus' in text.get('studycore','') and 'point = point * environment(me)->query("custom_home/study_bonus") / 100' in text.get('studycore',''),
 'bed_present':'/custom/home/obj/bed' in text.get('bedroom',''),
 'bed_60_seconds':'call_out("rest_tick", 60, me)' in text.get('bed',''),
 'bed_10_percent':'gain = maxv / 10' in text.get('bed',''),
 'bed_three_stats':'({ "gin", "kee", "sen" })' in text.get('bed',''),
 'bed_caps_effective':'cap = eff > 0 && eff < maxv ? eff : maxv' in text.get('bed',''),
 'future_room_entry':'enter' in text.get('hall','') and 'future_room' in text.get('hall',''),}
for k,v in checks.items():
    if not v: fail.append(k)
for k,s in text.items():
    for a,b in [('(',')'),('{','}'),('[',']')]:
        if s.count(a)!=s.count(b): fail.append(f'unbalanced:{k}:{a}{b}')
out={'version':(ROOT/'VERSION').read_text().strip(),'checks':checks,'failures':fail,'passed':not fail}
(ROOT/'reports/custom_home_audit.json').write_text(json.dumps(out,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(out,ensure_ascii=False,indent=2)); sys.exit(0 if not fail else 1)
