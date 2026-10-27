export class PlayerStateObserver {
  constructor(){
    this.state={score:{},scoreDetail:{attributes:{},stats:{},experience:[]},vitals:{},skills:[],inventory:[],inventoryMeta:{},navigation:[],events:[],lastCommand:null};
    this.mode=null;
  }
  begin(mode,command=mode){
    this.state.lastCommand=command;this.mode=mode;
    if(mode==='skills')this.state.skills=[];
    if(mode==='inventory'){this.state.inventory=[];this.state.inventoryMeta={};}
    if(mode==='hp')this.state.vitals={};
    if(mode==='score')this.state.scoreDetail={attributes:{},stats:{},experience:[]};
  }
  consume(text){
    const clean=String(text).replace(/\x1b\[[0-9;]*m/g,'');
    for(const line of clean.split(/\r\n|\n|\r/)){
      if(!line)continue;
      this.parseScore(line);
      if(this.mode==='score')this.parseScoreCanonical(line);
      if(['go','look'].includes(this.mode))this.parseNavigation(line);
      if(this.mode==='hp')this.parseHp(line);
      if(this.mode==='skills')this.parseSkill(line);
      if(this.mode==='inventory')this.parseInventory(line);
      this.parseEvent(line);
    }
  }
  parseScore(line){
    const labels={'等級':'level','形體':'hp','精':'jing','氣':'qi','神':'shen','食物':'food','飲水':'water','疲勞':'fatigue'};
    for(const [zh,key] of Object.entries(labels)){
      const m=line.match(new RegExp(zh+'[^0-9-]*(-?\\d+)'));
      if(m)this.state.score[key]=Number(m[1]);
    }
  }

  parseScoreCanonical(line){
    // Confirmed from cmds/usr/score.c. Only parse fields literally emitted there.
    const attrMap={'膂力':'str','膽識':'cor','悟性':'int','靈性':'spi','定力':'cps','機敏':'dex','根骨':'con','慧根':'wis'};
    for(const [zh,key] of Object.entries(attrMap)){
      const re=new RegExp(zh+'\\s*(-?\\d+)(?:\\s*\\(([+-]\\d+)\\))?');
      const m=line.match(re);
      if(m){
        const value=Number(m[1]),delta=m[2]===undefined?0:Number(m[2]);
        this.state.scoreDetail.attributes[key]={value,base:value-delta,delta};
      }
    }
    const statMap={'形體':'hp','精':'jing','氣':'qi','神':'shen','食物':'food','飲水':'water','疲勞':'fatigue'};
    for(const [zh,key] of Object.entries(statMap)){
      const m=line.match(new RegExp(zh+'\\s*(-?\\d+)\\/\\s*(-?\\d+)'));
      if(m)this.state.scoreDetail.stats[key]={current:Number(m[1]),maximum:Number(m[2])};
    }
    const lvl=line.match(/目前是等級\s*(\d+)/);
    if(lvl)this.state.scoreDetail.level=Number(lvl[1]);
    const karma=line.match(/目前有\s*(-?\d+)\s*點業力/)||line.match(/累積業力共\s*(-?\d+)\s*點/);
    if(karma)this.state.scoreDetail.karma=Number(karma[1]);
    // Experience rows: label value [remaining], two columns may appear on one line.
    const xp=/([^\s\[\]]{1,16})\s+(-?\d+)(?:\s+\[([0-9]+)\])?/g;
    if(line.includes('人物經驗/評價'))this.state.scoreDetail.experienceHeader=true;
    else if(this.state.scoreDetail.experienceHeader){
      let m;
      while((m=xp.exec(line))!==null){
        this.state.scoreDetail.experience.push({label:m[1],value:Number(m[2]),remaining:m[3]===undefined?null:Number(m[3]),raw:line});
      }
    }
  }
  parseNavigation(line){
    if(this.mode==='look' && line.includes('你現在什麼也看不到')){
      this.state.navigation.push({command:this.state.lastCommand,kind:'look_blocked',raw:line});return;
    }
    if(this.mode!=='go')return;
    const failures=[
      ['你要往哪個方向走','missing_direction'],
      ['要移動請你先用 halt 終止你正在做的事','busy'],
      ['你的負荷過重','over_encumbered'],
      ['你哪裡也去不了','no_environment'],
      ['你已經無路可逃了','no_escape_route'],
      ['這個方向沒有出路','no_exit'],
      ['這一個方向的出口有問題','broken_exit']
    ];
    for(const [needle,kind] of failures){
      if(line.includes(needle)){this.state.navigation.push({command:this.state.lastCommand,kind,raw:line});return;}
    }
    if(/你被.+擋住了/.test(line))this.state.navigation.push({command:this.state.lastCommand,kind:'blocked_by_enemy',raw:line});
    if(this.state.navigation.length>100)this.state.navigation=this.state.navigation.slice(-100);
  }

  parseHp(line){
    const labels={
      '形體':['hp','effective'],'精':['jing','effective'],'氣':['qi','effective'],'神':['shen','effective'],
      '食物':['food','maximum'],'飲水':['water','maximum'],'疲勞':['fatigue','maximum']
    };
    for(const [zh,[key,maxKind]] of Object.entries(labels)){
      const m=line.match(new RegExp(zh+'\\s*(-?\\d+)\\/(-?\\d+)'));
      if(m)this.state.vitals[key]={current:Number(m[1]),limit:Number(m[2]),limitKind:maxKind};
    }
  }
  parseSkill(line){
    // Canonical skills.c format ends in: " - <description> <effective> [(+/-delta)]".
    const m=line.match(/^\s*(.*?)\s+-\s+([^\s]+)\s+(\d+)(?:\s+\(([+-]\d+)\))?\s*$/);
    if(!m)return;
    let label=m[1].trim();
    const marker=label.startsWith('ˇ');
    if(marker)label=label.slice(1).trim();
    const desc=m[2],effectiveLevel=Number(m[3]),delta=m[4]===undefined?0:Number(m[4]);
    let baseName=null,skillId=null,mappedName=null,mappedId=null;
    const mapped=label.match(/^(.*?)\s+-\s+(.*?)\s+\(([^()]+)\)$/);
    if(mapped){baseName=mapped[1].trim();mappedName=mapped[2].trim();mappedId=mapped[3].trim();}
    else {
      const normal=label.match(/^(.*?)\s+\(([^()]+)\)$/);
      if(!normal)return;
      baseName=normal[1].trim();skillId=normal[2].trim();
    }
    this.state.skills.push({
      baseName,skillId,mappedName,mappedId,description:desc,effectiveLevel,
      rawLevel:effectiveLevel-delta,delta,mappedTargetMarker:marker,raw:line
    });
  }
  parseInventory(line){
    const header=line.match(/身上帶著下列這些東西\(負重\s*(\d+)%\)/);
    if(header){this.state.inventoryMeta.encumbrancePercent=Number(header[1]);this.state.inventoryMeta.seenHeader=true;return;}
    if(/身上沒有(?:任何東西|攜帶任何東西)/.test(line)){this.state.inventoryMeta.empty=true;this.state.inventoryMeta.seenHeader=true;return;}
    if(!this.state.inventoryMeta.seenHeader||!line.trim())return;
    const equipped=/^\s*ˇ/.test(line);
    let text=line.replace(/^\s*ˇ?\s*/,'').trim();
    const kept=text.endsWith('◎');
    if(kept)text=text.slice(0,-1).trimEnd();
    if(!text)return;
    this.state.inventory.push({text,equipped,kept,raw:line});
  }
  parseEvent(line){
    let kind=null;
    // Exact canonical lifecycle strings from std/char.c and obj/user.c.
    if(line.includes('你死了！！'))kind='death_self_exact';
    else if(line.includes('你已經魂飛魄散了！！！'))kind='ghost_destroyed_exact';
    else if(line.includes('你的眼前一黑，接著什麼也不知道了....'))kind='unconscious_self_exact';
    else if(line.includes('慢慢地你終於又有了知覺....'))kind='revive_self_exact';
    else if(line.includes('重新連線完畢。'))kind='reconnect_complete_exact';
    else if(line.includes('輸掉了這場比試。'))kind='fight_lost_exact';
    else if(line.includes('你贏了這場比試。'))kind='fight_won_exact';
    else if(/死亡|死了|斃命|倒地不起/.test(line))kind='death_observed';
    else if(/昏迷|失去知覺/.test(line))kind='unconscious_observed';
    else if(/受傷|傷害|擊中|攻擊/.test(line))kind='combat_text';
    if(kind)this.state.events.push({kind,text:line,time:Date.now()});
    if(this.state.events.length>100)this.state.events=this.state.events.slice(-100);
  }
}
