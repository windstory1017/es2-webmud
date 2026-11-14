// 觸發（trigger）與計時器（timer）。設定只存在這個瀏覽器（localStorage），可匯出／匯入。
//
// 觸發樣式採 zMUD 寫法：
//   *  任意文字        ?  任一個字
//   %d 數字            %n 可帶正負號的數字
//   %w 一個詞（中英文字、數字）   %a 同 %w
//   %s 空白            %x 非空白字元
//   ( ) 擷取成 %1～%9   {甲|乙} 二選一   [abc] 字元集合
//   ^ 行首  $ 行尾      ~ 讓下一個字元照字面比對
// 也可勾選「正規表示式」直接寫 JavaScript regex。
// 指令可用 ; 串接，%0 是整段符合的文字，%1～%9 是擷取到的文字。
//
// 網頁本地指令（不送到伺服器，輸入欄、觸發、計時器都能用；完整說明見 docs/WEB_ZMUD_COMMANDS.md）：
//   #wa 毫秒      等一下再送後面的指令（只寫 #wa 是 1 秒）
//   #3 指令       同一個指令重複 3 次（也可以 #3 {指令;指令}）
//   #dis／#con    中斷連線（不自動重連）／重新連線
//   #tr [on|off]  開關全部觸發      #t+ 類別／#t- 類別  開關某一組觸發
//   #sh／#echo 文字  只在自己畫面印一行字      #beep  叫一聲
//   #if (條件) {成立時} {不成立時}   #while (條件) {指令}   #loop 次數 {指令}   #case 數字 {一} {二}…
//   #var 名稱 值／#unvar 名稱   #math 名稱 運算式   #add 名稱 數字      指令裡寫 @名稱 帶入變數
//   #alias 名稱 {指令}／#unalias 名稱      #trigger {樣式} {指令} [類別]／#untrigger {樣式}
//   #alarm 秒數 {指令}   #abort   #gag [樣式]／#ungag 樣式   #color {樣式} 顏色／#uncolor 樣式
//   .3n2e         快速移動（n;n;n;e;e）
//   %len %random %time %left %right %pos %eval %if   函數

import {bgTimeout,bgInterval,clearBg} from './bg_timer.js';
import {splitStatements,readArgs,expandSpeedwalk,evaluate,expandText,formatValue,isNum} from './zscript.js';

const STORAGE_KEY='es2-automation-v1';
const MAX_AUTO_SENDS_PER_SECOND=20;
const TIMER_TICK_MS=250;
const MAX_USER_CHAIN_COMMANDS=9;   // 輸入欄一次最多送幾個指令（#wa 等本地指令不算）
const MAX_REPEAT=99;
const MAX_WAIT_MS=3600000;
const MAX_STEPS_WITHOUT_WAIT=2000; // 沒有 #wa 的情況下一串指令最多處理幾步（避免 #while 迴圈卡住網頁）
const MAX_LOOP=99;
// 角色狀態變數（讀右邊角色狀態視窗的數字），名稱 -> [狀態代碼, 目前值或最大值]
const STATUS_VARS={hp:['hp','current'],hpmax:['hp','max'],jing:['jing','current'],jingmax:['jing','max'],
  qi:['qi','current'],qimax:['qi','max'],shen:['shen','current'],shenmax:['shen','max'],
  gin:['jing','current'],ginmax:['jing','max'],kee:['qi','current'],keemax:['qi','max'],sen:['shen','current'],senmax:['shen','max'],
  food:['food','current'],foodmax:['food','max'],water:['water','current'],watermax:['water','max'],
  fatigue:['fatigue','current'],fatiguemax:['fatigue','max']};
// #color 的顏色名稱 -> ANSI 色碼
const COLOR_CODES={red:'31',green:'32',yellow:'33',blue:'34',magenta:'35',purple:'35',cyan:'36',white:'37',gray:'90',grey:'90',
  orange:'38;5;208',pink:'38;5;213',
  紅:'31',綠:'32',黃:'33',藍:'34',紫:'35',青:'36',白:'37',灰:'90',橘:'38;5;208',粉紅:'38;5;213'};
export function colorCode(name){
  const n=String(name||'').trim().toLowerCase();
  const bright=/^(?:hi|bold|亮)/.test(n);
  const base=n.replace(/^(?:hi|bold|亮)\s*/,'');
  const c=COLOR_CODES[base];
  if(!c)return null;
  if(bright&&/^3[1-7]$/.test(c))return String(Number(c)+60);
  if(bright&&c==='90')return '97';
  return c;
}

const esc=s=>String(s??'').replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
const uid=()=>Date.now().toString(36)+Math.random().toString(36).slice(2,7);

export function compilePattern(pattern,{regex=false,caseSensitive=false}={}){
  const flags='u'+(caseSensitive?'':'i');
  if(regex)return new RegExp(pattern,flags);
  const p=String(pattern);
  let out='';
  for(let i=0;i<p.length;i++){
    const c=p[i];
    if(c==='~'&&i+1<p.length){out+=escapeRe(p[++i]);continue;}
    if(c==='^'&&i===0){out+='^';continue;}
    if(c==='$'&&i===p.length-1){out+='$';continue;}
    if(c==='*'){out+='.*';continue;}
    if(c==='?'){out+='.';continue;}
    if(c==='('||c===')'){out+=c;continue;}
    if(c==='{'){
      const end=p.indexOf('}',i);
      if(end>i){out+='(?:'+p.slice(i+1,end).split('|').map(escapeRe).join('|')+')';i=end;continue;}
    }
    if(c==='['){
      const end=p.indexOf(']',i);
      if(end>i){out+='['+p.slice(i+1,end).replace(/[\\\]]/g,'\\$&')+']';i=end;continue;}
    }
    if(c==='%'&&i+1<p.length){
      const k=p[i+1];
      const map={d:'\\d+',n:'[+-]?\\d+',w:'[\\p{L}\\p{N}_]+',a:'[\\p{L}\\p{N}_]+',s:'\\s+',x:'\\S+'};
      if(map[k]){out+=map[k];i++;continue;}
    }
    out+=escapeRe(c);
  }
  return new RegExp(out,flags);
}
function escapeRe(c){return c.replace(/[.*+?^${}()|[\]\\\/]/g,'\\$&');}

export function expandCommand(template,match){
  return String(template).replace(/%(\d)/g,(_,n)=>match[Number(n)]??'');
}

// 把一行拆成一個個指令（大括號裡的 ; 不拆），並展開 #3 e、#3 {e;n} 這種重複寫法。
export function splitCommandLine(line){
  const out=[];
  for(const part of splitStatements(line)){
    const m=part.match(/^#(\d+)\s+(.+)$/s);
    if(m){
      const n=Math.min(MAX_REPEAT,Number(m[1]));
      const body=m[2].trim();
      const inner=/^\{[\s\S]*\}$/.test(body)?splitCommandLine(body.slice(1,-1)):[body];
      for(let i=0;i<n;i++)out.push(...inner);
    }
    else out.push(part);
  }
  return out;
}
// #wa／#wait 回傳要等的毫秒數，不是等待指令就回傳 null。
export function parseWait(part){
  const m=String(part).trim().match(/^#wa(?:it)?(?:\s+(\d+))?\s*$/i);
  if(!m)return null;
  return Math.min(MAX_WAIT_MS,m[1]===undefined?1000:Number(m[1]));
}

function defaultState(){return {triggersEnabled:true,paused:false,disabledClasses:[],triggers:[],timers:[],aliases:{},vars:{},gags:[],colors:[]};}
function normalizeRule(r,withColor){
  const x={id:r.id||uid(),pattern:String(r.pattern||''),caseSensitive:!!r.caseSensitive,regex:!!r.regex};
  if(withColor)x.color=String(r.color||'');
  return x;
}
const plainObject=(o,fn)=>Object.fromEntries(Object.entries(o&&typeof o==='object'&&!Array.isArray(o)?o:{}).map(([k,v])=>[String(k),fn(v)]));
function normalizeTrigger(t){
  return {id:t.id||uid(),pattern:String(t.pattern||''),command:String(t.command||''),cls:String(t.cls||''),
    enabled:t.enabled!==false,caseSensitive:!!t.caseSensitive,regex:!!t.regex,prompt:!!t.prompt};
}
function normalizeTimer(t){
  const seconds=Math.max(1,Math.floor(Number(t.seconds)||0));
  const fireAt=Math.min(seconds,Math.max(0,Math.floor(Number(t.fireAt)||0)));
  return {id:t.id||uid(),name:String(t.name||''),seconds,fireAt,command:String(t.command||''),loop:!!t.loop};
}

function defaultBeep(){
  try{
    const AC=globalThis.AudioContext||globalThis.webkitAudioContext;if(!AC)return;
    const ac=defaultBeep.ac||(defaultBeep.ac=new AC());
    const o=ac.createOscillator(),g=ac.createGain();
    o.frequency.value=880;g.gain.value=0.08;o.connect(g);g.connect(ac.destination);
    o.start();o.stop(ac.currentTime+0.15);
  }catch(_){}
}

export class Automation{
  constructor({sendCommand,sendUserCommand,notify,isBlocked,disconnect,connect,getStatus,beep}){
    this.sendCommand=sendCommand;   // (command) => void，觸發／計時器送出的指令（畫面上照樣顯示）
    this.sendUserCommand=sendUserCommand||sendCommand; // 玩家自己在輸入欄打的指令
    this.disconnect=disconnect||(()=>false); // #dis：回傳 false 表示本來就沒連線
    this.connect=connect||(()=>false);       // #con：回傳 false 表示已經連線中
    this.pending=new Set();         // 被 #wa 擋住、還在等的指令串
    this.notify=notify;             // (message) => void，在畫面印一行系統訊息
    this.isBlocked=isBlocked||(()=>false); // 輸入密碼、編輯文章時不自動送指令
    this.getStatus=getStatus||(()=>({}));   // 角色狀態：{hp:{current,max}, jing:…}
    this.beep=beep||defaultBeep;
    this.ctx={getVar:n=>this.getVar(n),hasVar:n=>this.hasVar(n)};
    this.state=this.load();
    this.compiled=new Map();
    this.lineBuffer='';
    this.promptFired=new Set();     // 這一行尚未換行時已經觸發過的 trigger id
    this.sendTimes=[];
    this.running=new Map();         // timer id -> {startedAt, fired}
    this.tickTimer=null;
    this.listeners=new Set();
  }

  // ---------- 儲存 ----------
  load(){
    try{
      const x=JSON.parse(localStorage.getItem(STORAGE_KEY)||'null');
      if(x&&typeof x==='object'){
        const s=defaultState();
        s.triggersEnabled=x.triggersEnabled!==false;
        s.paused=!!x.paused;
        s.disabledClasses=Array.isArray(x.disabledClasses)?x.disabledClasses.map(String):[];
        s.triggers=Array.isArray(x.triggers)?x.triggers.map(normalizeTrigger):[];
        s.timers=Array.isArray(x.timers)?x.timers.map(normalizeTimer):[];
        s.aliases=plainObject(x.aliases,String);
        s.vars=plainObject(x.vars,v=>typeof v==='number'?v:String(v));
        s.gags=Array.isArray(x.gags)?x.gags.map(r=>normalizeRule(r,false)):[];
        s.colors=Array.isArray(x.colors)?x.colors.map(r=>normalizeRule(r,true)):[];
        return s;
      }
    }catch(_){}
    return defaultState();
  }
  save(){
    try{localStorage.setItem(STORAGE_KEY,JSON.stringify(this.state));}catch(_){}
    this.compiled.clear();
    this.emit();
  }
  onChange(fn){this.listeners.add(fn);}
  emit(){for(const fn of this.listeners)fn();}

  exportJSON(){
    const {triggersEnabled,disabledClasses,triggers,timers,aliases,vars,gags,colors}=this.state;
    return JSON.stringify({format:'es2-automation',version:2,triggersEnabled,disabledClasses,triggers,timers,aliases,vars,gags,colors},null,2);
  }
  importJSON(text){
    const x=JSON.parse(text);
    if(!x||typeof x!=='object'||(!Array.isArray(x.triggers)&&!Array.isArray(x.timers)))
      throw new Error('檔案格式不正確');
    this.stopAllTimers();
    this.state.triggers=(x.triggers||[]).map(normalizeTrigger);
    this.state.timers=(x.timers||[]).map(normalizeTimer);
    this.state.disabledClasses=Array.isArray(x.disabledClasses)?x.disabledClasses.map(String):[];
    if(typeof x.triggersEnabled==='boolean')this.state.triggersEnabled=x.triggersEnabled;
    // 舊版匯出檔沒有這幾項：沒有的就保留目前的設定。
    if(x.aliases)this.state.aliases=plainObject(x.aliases,String);
    if(x.vars)this.state.vars=plainObject(x.vars,v=>typeof v==='number'?v:String(v));
    if(Array.isArray(x.gags))this.state.gags=x.gags.map(r=>normalizeRule(r,false));
    if(Array.isArray(x.colors))this.state.colors=x.colors.map(r=>normalizeRule(r,true));
    this.save();
    return {triggers:this.state.triggers.length,timers:this.state.timers.length,aliases:Object.keys(this.state.aliases).length};
  }

  // ---------- 開關 ----------
  setTriggersEnabled(on){this.state.triggersEnabled=!!on;this.save();}
  setPaused(on){this.state.paused=!!on;if(this.state.paused)this.cancelPending();this.save();}
  toggleClass(cls,on){
    const set=new Set(this.state.disabledClasses);
    if(on)set.delete(cls);else set.add(cls);
    this.state.disabledClasses=[...set];this.save();
  }
  classes(){return [...new Set(this.state.triggers.map(t=>t.cls).filter(Boolean))].sort();}

  // ---------- 變數 ----------
  statusValue(name){
    const k=STATUS_VARS[String(name).toLowerCase()];if(!k)return undefined;
    const v=this.getStatus()?.[k[0]];
    return v?(Number(v[k[1]])||0):0;
  }
  hasVar(name){return STATUS_VARS[String(name).toLowerCase()]!==undefined||Object.prototype.hasOwnProperty.call(this.state.vars,name);}
  getVar(name){
    const st=this.statusValue(name);if(st!==undefined)return st;
    return Object.prototype.hasOwnProperty.call(this.state.vars,name)?this.state.vars[name]:'';
  }
  setVar(name,value){
    if(STATUS_VARS[String(name).toLowerCase()]){this.notify(`@${name} 是角色狀態，不能自己設定。`);return false;}
    if(!/^[\p{L}\p{N}_]+$/u.test(name)){this.notify(`變數名稱「${name}」不能有空白或符號。`);return false;}
    this.state.vars[name]=typeof value==='number'?value:(isNum(value)?Number(value):String(value));
    this.saveQuiet();return true;
  }
  // 變數常常在觸發裡更新：只存檔、不重建管理視窗以外的東西。
  saveQuiet(){try{localStorage.setItem(STORAGE_KEY,JSON.stringify(this.state));}catch(_){}this.emit();}

  // ---------- 隱藏與上色 ----------
  ruleRegex(r){
    const key='rule:'+r.id;
    let re=this.compiled.get(key);
    if(re===undefined){try{re=r.pattern?compilePattern(r.pattern,r):null;}catch(_){re=null;}this.compiled.set(key,re);}
    return re;
  }
  isGagged(plain){
    const text=plain.replace(/^(?:[>＞]\s*)+/,'');
    return this.state.gags.some(r=>{const re=this.ruleRegex(r);return re&&(re.lastIndex=0,re.test(text));});
  }
  // 在含 ANSI 色碼的一行裡，把符合 #color 樣式的文字上色；色碼之外的位置不變。
  colorize(raw){
    if(!this.state.colors.length)return raw;
    const chars=[],pos=[];
    const sgr=/\x1b\[[0-9;]*m/y;
    for(let i=0;i<raw.length;){
      sgr.lastIndex=i;const m=sgr.exec(raw);
      if(m){i+=m[0].length;continue;}
      chars.push(raw[i]);pos.push(i);i++;
    }
    const plain=chars.join('');
    const marks=new Array(plain.length).fill(null);
    for(const r of this.state.colors){
      const code=colorCode(r.color),re=this.ruleRegex(r);if(!code||!re)continue;
      const g=new RegExp(re.source,re.flags.includes('g')?re.flags:re.flags+'g');
      let m;while((m=g.exec(plain))){
        if(!m[0]){g.lastIndex++;continue;}
        for(let k=m.index;k<m.index+m[0].length;k++)if(marks[k]===null)marks[k]=code;
      }
    }
    if(marks.every(x=>x===null))return raw;
    let out='',active=[],cur=null,ci=0;
    for(let i=0;i<raw.length;){
      sgr.lastIndex=i;const m=sgr.exec(raw);
      if(m){
        const seq=m[0];
        if(/^\x1b\[0?m$/.test(seq))active=[];else active.push(seq);
        if(cur===null)out+=seq;           // 上色範圍內先不套遊戲原本的顏色，結束後再補回
        i+=seq.length;continue;
      }
      const want=marks[ci];
      if(want!==cur){
        if(want!==null)out+=`\x1b[${want}m`;
        else out+='\x1b[0m'+active.join('');
        cur=want;
      }
      out+=raw[i];i++;ci++;
    }
    if(cur!==null)out+='\x1b[0m'+active.join('');
    return out;
  }
  // 收到遊戲文字時呼叫：跑觸發，並回傳要顯示的文字（拿掉 #gag 的行、套上 #color）。
  processOutput(text,cleanText){
    const gaggedLines=this.feed(text,cleanText)||[];
    if(!this.state.gags.length&&!this.state.colors.length&&!gaggedLines.some(Boolean))return text;
    const pieces=String(text).split(/(\r\n|\n|\r)/);
    let out='',line=0;
    for(let i=0;i<pieces.length;i+=2){
      const seg=pieces[i],nl=pieces[i+1]??'';
      const complete=i+1<pieces.length;
      const gag=(complete&&gaggedLines[line])||(seg&&this.isGagged(cleanText(seg)));
      if(complete)line++;
      if(gag)continue;
      out+=this.colorize(seg)+nl;
    }
    return out;
  }

  // #tr、#t+、#t-、#sh、#dis、#con。回傳 true 表示是本地指令，不送到伺服器。
  // （#wa 和 #3 由 run() 處理。）
  handleClientCommand(raw){
    const text=String(raw).trim();
    let m=text.match(/^#tr(?:\s+(on|off))?\s*$/i);
    if(m){
      const next=m[1]?m[1].toLowerCase()==='on':!this.state.triggersEnabled;
      this.setTriggersEnabled(next);
      this.notify(`觸發已全部${next?'開啟':'關閉'}。`);
      return true;
    }
    m=text.match(/^#t([+-])\s*(.*)$/i);
    if(m){
      const cls=m[2].trim(),on=m[1]==='+';
      if(!cls){this.notify('請寫上要開關的觸發類別。');return true;}
      this.toggleClass(cls,on);
      const n=this.state.triggers.filter(t=>t.cls===cls).length;
      this.notify(n?`觸發類別「${cls}」已${on?'開啟':'關閉'}（${n} 個）。`:`沒有類別為「${cls}」的觸發。`);
      return true;
    }
    m=text.match(/^#(?:sh|show|echo)(?:\s+([\s\S]*))?$/i);
    if(m){this.notify(m[1]||'');return true;}
    if(/^#beep\s*$/i.test(text)){this.beep();return true;}
    if(/^#dis(?:connect)?\s*$/i.test(text)){
      if(!this.disconnect())this.notify('目前沒有連線。');
      return true;
    }
    if(/^#con(?:nect)?\s*$/i.test(text)){
      if(!this.connect())this.notify('已經連線中。');
      return true;
    }
    return false;
  }

  // ---------- 觸發 ----------
  upsertTrigger(t){
    const x=normalizeTrigger(t);
    compilePattern(x.pattern,x); // 樣式有錯就在這裡丟出例外
    const i=this.state.triggers.findIndex(y=>y.id===x.id);
    if(i>=0)this.state.triggers[i]=x;else this.state.triggers.push(x);
    this.save();return x;
  }
  removeTrigger(id){this.state.triggers=this.state.triggers.filter(t=>t.id!==id);this.save();}
  regexFor(t){
    let re=this.compiled.get(t.id);
    if(re===undefined){
      try{re=t.pattern?compilePattern(t.pattern,t):null;}catch(_){re=null;}
      this.compiled.set(t.id,re);
    }
    return re;
  }
  activeTriggers(){
    if(!this.state.triggersEnabled||this.state.paused)return [];
    const off=new Set(this.state.disabledClasses);
    return this.state.triggers.filter(t=>t.enabled&&!(t.cls&&off.has(t.cls)));
  }

  // 收到遊戲文字（已去掉 @@WEBHUD 資料，可含 ANSI 色碼；cleanText 由呼叫端提供）。
  // 回傳這段文字裡每一個完整行是否被觸發的 #gag 隱藏。
  feed(text,cleanText){
    if(!this.state.triggers.length)return [];
    this.lineBuffer+=String(text);
    const parts=this.lineBuffer.split(/\r\n|\n|\r/);
    this.lineBuffer=parts.pop()||'';
    if(this.lineBuffer.length>4000)this.lineBuffer=this.lineBuffer.slice(-4000);
    const active=this.activeTriggers();
    const gagged=[];
    for(const line of parts){
      this.gagThisLine=false;
      if(active.length)this.matchLine(cleanText(line),active,this.promptFired);
      gagged.push(this.gagThisLine);
      this.promptFired=new Set();
    }
    this.gagThisLine=false;
    // 沒有換行的提示行（例如「你要買嗎？」），只比對勾了「提示行」的觸發。
    if(this.lineBuffer&&active.length){
      const promptTriggers=active.filter(t=>t.prompt&&!this.promptFired.has(t.id));
      if(promptTriggers.length){
        const fired=this.matchLine(cleanText(this.lineBuffer),promptTriggers,new Set());
        for(const id of fired)this.promptFired.add(id);
      }
    }
    this.gagThisLine=false;
    return gagged;
  }
  matchLine(line,triggers,skip){
    const fired=[];
    // 遊戲的「> 」提示字元常和下一則訊息黏在同一行，比對前先拿掉，^ 行首才會準。
    const plain=line.replace(/^(?:[>＞]\s*)+/,'').replace(/\s+$/,'');
    if(!plain.trim())return fired;
    for(const t of triggers){
      if(skip.has(t.id))continue;
      const re=this.regexFor(t);if(!re)continue;
      re.lastIndex=0;
      const m=re.exec(plain);if(!m)continue;
      fired.push(t.id);
      if(t.command){
        const cmd=expandCommand(t.command,m);
        // 觸發指令裡有單獨的 #gag：這一行不顯示。
        if(splitStatements(cmd).some(x=>/^#gag\s*$/i.test(x)))this.gagThisLine=true;
        this.run(cmd,`觸發「${t.pattern}」`);
      }
    }
    return fired;
  }

  // 執行一串以 ; 分隔的指令（可含 #wa、#3、#if 等本地指令）。
  // user：玩家在輸入欄打的。一次最多送 9 個、不受每秒 20 個的保險限制。
  run(commandLine,source,{user=false}={}){
    const parts=splitCommandLine(commandLine);
    if(user&&!parts.length){this.sendUserCommand(String(commandLine));return;} // 空白行照送（例如按 Enter 繼續）
    this.step({parts,i:0,source,user,sent:0,timer:null,steps:0},true);
  }
  // 在目前位置插入一段指令（#if、#loop、縮寫等展開出來的）。
  splice(job,text){job.parts.splice(job.i,0,...splitCommandLine(text));}
  step(job,first){
    while(job.i<job.parts.length){
      const raw=job.parts[job.i++];
      if(++job.steps>MAX_STEPS_WITHOUT_WAIT){
        this.notify(`指令重複太多次（${job.source||'輸入欄'}），已停止，請檢查 #while、#loop 或縮寫是否會無限循環。`);
        return;
      }
      // 整段大括號：展開裡面的指令
      if(/^\{[\s\S]*\}$/.test(raw)){this.splice(job,raw.slice(1,-1));continue;}
      const ctl=this.control(raw,job);
      if(ctl==='abort')return;
      if(ctl)continue;
      const part=expandText(raw,this.ctx);
      const wait=parseWait(part);
      if(wait!==null){
        if(job.i>=job.parts.length)return; // 最後一個是 #wa，不用等
        this.pending.add(job);
        job.steps=0;
        job.timer=bgTimeout(()=>{this.pending.delete(job);this.step(job,false);},wait); // 背景分頁也準時
        return;
      }
      const walk=expandSpeedwalk(part);
      if(walk){job.parts.splice(job.i,0,...walk);continue;}
      if(/^#\d+\s/.test(part)){this.splice(job,part);continue;}
      if(this.handleClientCommand(part))continue;
      const alias=this.aliasExpansion(part);
      if(alias!==null){this.splice(job,alias);continue;}
      // 玩家當下打的指令照常送（沒連線時會顯示提示）；等過 #wa 之後和自動指令則先確認現在能送。
      if((!job.user||!first)&&this.isBlocked())return;
      if(job.user){
        if(job.sent>=MAX_USER_CHAIN_COMMANDS){this.notify('你的動作太快了...');return;}
        job.sent++;
        this.sendUserCommand(part);
        continue;
      }
      const now=Date.now();
      this.sendTimes=this.sendTimes.filter(x=>now-x<1000);
      if(this.sendTimes.length>=MAX_AUTO_SENDS_PER_SECOND){
        // 觸發互相引發、無限循環時的保險：全部暫停，等玩家檢查設定。
        this.setPaused(true);
        this.notify(`自動指令一秒內超過 ${MAX_AUTO_SENDS_PER_SECOND} 個（${job.source}），已暫停全部觸發與計時器，請檢查設定後再按輸入欄左邊的按鈕恢復。`);
        return;
      }
      this.sendTimes.push(now);
      this.sendCommand(part);
    }
  }

  // ---------- 需要原文（大括號區塊、運算式）的本地指令 ----------
  // 回傳 'abort' 結束這串指令、true 表示處理完畢、false 表示不是這類指令。
  control(raw,job){
    const m=raw.match(/^#([a-z]+)(?:\s+([\s\S]*))?$/i);
    if(!m)return false;
    const cmd=m[1].toLowerCase(),rest=m[2]||'';
    const args=readArgs(rest);
    const val=k=>args[k]?.value??'';
    switch(cmd){
      case 'if':{
        if(!args.length){this.notify('#if 要寫條件，例如 #if (@hp < 50) {指令}。');return true;}
        const ok=this.truthy(val(0));
        const block=ok?val(1):val(2);
        if(block)this.splice(job,block);
        return true;
      }
      case 'while':{
        if(this.truthy(val(0))&&val(1)){job.parts.splice(job.i,0,...splitCommandLine(val(1)),raw);}
        return true;
      }
      case 'loop':{
        const spec=String(val(0)).split(',').map(x=>Math.trunc(Number(evaluate(x,this.ctx))||0));
        let [a,b]=spec.length>1?spec:[1,spec[0]];
        const body=val(1);if(!body)return true;
        const step=a<=b?1:-1,list=[];
        for(let n=a,count=0;step>0?n<=b:n>=b;n+=step){
          if(++count>MAX_LOOP){this.notify(`#loop 最多重複 ${MAX_LOOP} 次。`);break;}
          list.push(...splitCommandLine(body.replace(/%(?:i|repeatnum)\b/gi,String(n))));
        }
        job.parts.splice(job.i,0,...list);
        return true;
      }
      case 'case':{
        const k=Math.trunc(Number(this.eval(val(0)))||0);
        if(k>=1&&args[k])this.splice(job,args[k].value);
        return true;
      }
      case 'abort':return 'abort';
      case 'var':case 'variable':{
        if(!args.length){
          const list=Object.entries(this.state.vars);
          this.notify(list.length?'變數：\n'+list.map(([k,v])=>`  @${k} = ${formatValue(v)}`).join('\n'):'目前沒有自訂變數。');
          return true;
        }
        const name=String(val(0)).replace(/^@/,'');
        if(args.length===1){this.notify(this.hasVar(name)?`@${name} = ${formatValue(this.getVar(name))}`:`沒有變數 @${name}。`);return true;}
        const value=args.slice(1).map(a=>a.kind==='w'?expandText(a.value,this.ctx):a.kind==='{'||a.kind==='"'?expandText(a.value,this.ctx):formatValue(this.eval(a.value))).join(' ');
        this.setVar(name,value);
        return true;
      }
      case 'unvar':case 'unvariable':{
        const name=String(val(0)).replace(/^@/,'');
        if(Object.prototype.hasOwnProperty.call(this.state.vars,name)){delete this.state.vars[name];this.saveQuiet();this.notify(`已刪除變數 @${name}。`);}
        else this.notify(`沒有變數 @${name}。`);
        return true;
      }
      case 'math':{
        const name=String(val(0)).replace(/^@/,'');
        if(!name||args.length<2){this.notify('#math 的寫法：#math 名稱 運算式，例如 #math total @a+@b。');return true;}
        this.setVar(name,this.eval(rest.slice(args[0].end)));
        return true;
      }
      case 'add':{
        const name=String(val(0)).replace(/^@/,'');
        if(!name){this.notify('#add 的寫法：#add 名稱 數字，例如 #add kills 1。');return true;}
        const n=args.length>1?Number(this.eval(rest.slice(args[0].end)))||0:1;
        this.setVar(name,(Number(this.getVar(name))||0)+n);
        return true;
      }
      case 'alias':case 'al':{
        if(!args.length){
          const list=Object.entries(this.state.aliases);
          this.notify(list.length?'縮寫：\n'+list.map(([k,v])=>`  ${k} = ${v}`).join('\n'):'目前沒有縮寫。');
          return true;
        }
        const name=String(val(0));
        if(args.length===1){const v=this.findAlias(name);this.notify(v!==null?`${name} = ${v}`:`沒有縮寫「${name}」。`);return true;}
        if(name.startsWith('#')||/[\s;{}]/.test(name)){this.notify('縮寫名稱不能用 # 開頭，也不能有空白或 ; { }。');return true;}
        const key=Object.keys(this.state.aliases).find(k=>k.toLowerCase()===name.toLowerCase());
        if(key)delete this.state.aliases[key];
        this.state.aliases[name]=args[1].kind==='{'?args[1].value:rest.slice(args[0].end).trim();
        this.save();this.notify(`已設定縮寫「${name}」。`);
        return true;
      }
      case 'unalias':case 'unal':{
        const name=String(val(0));
        const key=Object.keys(this.state.aliases).find(k=>k.toLowerCase()===name.toLowerCase());
        if(key){delete this.state.aliases[key];this.save();this.notify(`已刪除縮寫「${key}」。`);}
        else this.notify(`沒有縮寫「${name}」。`);
        return true;
      }
      case 'trigger':case 'tr':{
        if(cmd==='tr'&&!(args[0]&&args[0].kind==='{'))return false; // #tr／#tr on／#tr off 是開關全部觸發
        if(args.length<2){this.notify('#trigger 的寫法：#trigger {樣式} {指令} [類別]。');return true;}
        try{
          this.upsertTrigger({pattern:val(0),command:val(1),cls:val(2)});
          this.notify(`已新增觸發「${val(0)}」。`);
        }catch(err){this.notify('樣式有誤：'+err.message);}
        return true;
      }
      case 'untrigger':case 'untr':{
        const p=val(0),before=this.state.triggers.length;
        this.state.triggers=this.state.triggers.filter(t=>t.pattern!==p);
        const n=before-this.state.triggers.length;
        if(n)this.save();
        this.notify(n?`已刪除 ${n} 個樣式為「${p}」的觸發。`:`沒有樣式為「${p}」的觸發。`);
        return true;
      }
      case 'alarm':{
        const sec=Number(String(val(0)).replace(/^\+/,''));
        if(!(sec>0)||!val(1)){this.notify('#alarm 的寫法：#alarm 秒數 {指令}，例如 #alarm 30 {eat dumpling}。');return true;}
        const wait=Math.min(MAX_WAIT_MS,Math.round(sec*1000));
        this.step({parts:[`#wa ${wait}`,...splitCommandLine(val(1))],i:0,source:`#alarm ${val(0)}`,user:false,sent:0,timer:null,steps:0},false);
        return true;
      }
      case 'gag':{
        if(!args.length)return true; // 觸發裡單獨的 #gag：隱藏觸發的那一行（在 matchLine 處理）
        const p=val(0);
        if(this.state.gags.some(r=>r.pattern===p)){this.notify(`「${p}」已經在隱藏清單裡。`);return true;}
        try{compilePattern(p);}catch(err){this.notify('樣式有誤：'+err.message);return true;}
        this.state.gags.push(normalizeRule({pattern:p},false));this.save();
        this.notify(`符合「${p}」的行之後不再顯示。`);
        return true;
      }
      case 'ungag':{
        const p=val(0),before=this.state.gags.length;
        this.state.gags=this.state.gags.filter(r=>r.pattern!==p);
        if(before!==this.state.gags.length){this.save();this.notify(`已取消隱藏「${p}」。`);}
        else this.notify(`隱藏清單裡沒有「${p}」。`);
        return true;
      }
      case 'color':case 'co':{
        if(args.length<2){this.notify('#color 的寫法：#color {樣式} 顏色，例如 #color {告訴你} 亮黃。');return true;}
        const p=val(0),color=val(1);
        if(!colorCode(color)){this.notify(`看不懂顏色「${color}」。可以用：紅 綠 黃 藍 紫 青 白 灰 橘 粉紅，前面加「亮」更亮；也可以用英文 red、hi yellow。`);return true;}
        try{compilePattern(p);}catch(err){this.notify('樣式有誤：'+err.message);return true;}
        this.state.colors=this.state.colors.filter(r=>r.pattern!==p);
        this.state.colors.push(normalizeRule({pattern:p,color},true));this.save();
        this.notify(`符合「${p}」的文字之後顯示成${color}。`);
        return true;
      }
      case 'uncolor':{
        const p=val(0),before=this.state.colors.length;
        this.state.colors=this.state.colors.filter(r=>r.pattern!==p);
        if(before!==this.state.colors.length){this.save();this.notify(`已取消「${p}」的上色。`);}
        else this.notify(`上色清單裡沒有「${p}」。`);
        return true;
      }
    }
    return false;
  }
  eval(expr){return evaluate(expr,this.ctx);}
  truthy(expr){const v=this.eval(expr);return typeof v==='number'?v!==0:(v!==''&&v!=='0');}

  // ---------- 縮寫 ----------
  findAlias(name){
    const n=String(name).toLowerCase();
    const key=Object.keys(this.state.aliases).find(k=>k.toLowerCase()===n);
    return key===undefined?null:this.state.aliases[key];
  }
  // 指令的第一個字是縮寫時回傳展開後的指令，否則回傳 null。
  aliasExpansion(part){
    if(part.startsWith('#'))return null;
    const m=part.match(/^(\S+)(?:\s+([\s\S]*))?$/);if(!m)return null;
    const body=this.findAlias(m[1]);if(body===null)return null;
    const restText=(m[2]||'').trim();
    const words=readArgs(restText).map(a=>a.value);
    if(!/%(?:\d|\*)/.test(body))return restText?`${body} ${restText}`:body;
    return body.replace(/%\*/g,restText).replace(/%(\d)/g,(_,n)=>n==='0'?part:(words[Number(n)-1]??''));
  }

  // 丟掉所有被 #wa 擋住、還沒送出的指令（斷線、暫停時）。
  cancelPending(){
    for(const job of this.pending)clearBg(job.timer);
    this.pending.clear();
  }

  // ---------- 計時器 ----------
  upsertTimer(t){
    const x=normalizeTimer(t);
    if(x.fireAt>x.seconds)throw new Error('「剩幾秒時執行」不能大於倒數秒數');
    const i=this.state.timers.findIndex(y=>y.id===x.id);
    if(i>=0)this.state.timers[i]=x;else this.state.timers.push(x);
    if(this.running.has(x.id))this.startTimer(x.id); // 改了設定就從頭開始倒數
    this.save();return x;
  }
  removeTimer(id){this.stopTimer(id);this.state.timers=this.state.timers.filter(t=>t.id!==id);this.save();}
  startTimer(id){
    this.running.set(id,{startedAt:Date.now(),fired:false});
    this.ensureTicking();this.emit();
  }
  stopTimer(id){
    this.running.delete(id);
    if(!this.running.size&&this.tickTimer){clearBg(this.tickTimer);this.tickTimer=null;}
    this.emit();
  }
  stopAllTimers(){for(const id of [...this.running.keys()])this.stopTimer(id);}
  remaining(id){
    const r=this.running.get(id),t=this.state.timers.find(x=>x.id===id);
    if(!r||!t)return null;
    return Math.max(0,t.seconds-(Date.now()-r.startedAt)/1000);
  }
  ensureTicking(){
    if(this.tickTimer)return;
    this.tickTimer=bgInterval(()=>this.tick(),TIMER_TICK_MS); // 網頁在背景也照常倒數
  }
  tick(){
    const now=Date.now();
    for(const [id,r] of [...this.running]){
      const t=this.state.timers.find(x=>x.id===id);
      if(!t){this.running.delete(id);continue;}
      const left=t.seconds-(now-r.startedAt)/1000;
      if(!r.fired&&left<=t.fireAt){
        r.fired=true;
        // 暫停時照常倒數，只是不送指令。
        if(!this.state.paused&&t.command)this.run(t.command,`計時器「${t.name||t.command}」`);
      }
      if(left<=0){
        if(t.loop){r.startedAt+=t.seconds*1000;r.fired=false;}
        else this.running.delete(id);
      }
    }
    if(!this.running.size&&this.tickTimer){clearBg(this.tickTimer);this.tickTimer=null;}
    this.emit();
  }
}

// ---------- 管理視窗 ----------
export function setupAutomationUI(auto,{dialog,toggleButton,openButtons}){
  if(!dialog)return;
  let tab='triggers';
  let editingTimer=null;
  const $=s=>dialog.querySelector(s);

  function renderToggle(){
    if(!toggleButton)return;
    const paused=auto.state.paused;
    toggleButton.classList.toggle('is-paused',paused);
    toggleButton.textContent=paused?'暫停':'自動';
    const tr=auto.state.triggersEnabled?'開':'關';
    toggleButton.title=paused
      ? `自動功能已暫停（觸發與計時器都不送指令）。按一下恢復。`
      : `自動功能運作中（觸發：${tr}，#tr 可切換）。按一下全部暫停。`;
    toggleButton.setAttribute('aria-pressed',paused?'true':'false');
  }

  function renderTabs(){
    for(const b of dialog.querySelectorAll('[data-auto-tab]')){
      const on=b.dataset.autoTab===tab;
      b.classList.toggle('active',on);b.setAttribute('aria-selected',on?'true':'false');
    }
    $('#autoTriggerPane').hidden=tab!=='triggers';
    $('#autoTimerPane').hidden=tab!=='timers';
    $('#autoScriptPane').hidden=tab!=='script';
  }

  function renderScript(){
    const del=(kind,key)=>`<td class="auto-actions"><button type="button" data-script-del="${kind}" data-key="${esc(key)}">刪除</button></td>`;
    const empty=(n,text)=>`<tr><td colspan="${n}" class="auto-muted">${text}</td></tr>`;
    const al=Object.entries(auto.state.aliases);
    $('#autoAliasRows').innerHTML=al.length?al.map(([k,v])=>`<tr><td class="auto-mono">${esc(k)}</td><td class="auto-mono">${esc(v)}</td>${del('alias',k)}</tr>`).join(''):empty(3,'尚未設定縮寫。');
    const vs=Object.entries(auto.state.vars);
    $('#autoVarRows').innerHTML=vs.length?vs.map(([k,v])=>`<tr><td class="auto-mono">@${esc(k)}</td><td class="auto-mono">${esc(String(v))}</td>${del('var',k)}</tr>`).join(''):empty(3,'尚未設定變數。');
    $('#autoGagRows').innerHTML=auto.state.gags.length?auto.state.gags.map(r=>`<tr><td class="auto-mono">${esc(r.pattern)}</td>${del('gag',r.id)}</tr>`).join(''):empty(2,'沒有隱藏的樣式。');
    $('#autoColorRows').innerHTML=auto.state.colors.length?auto.state.colors.map(r=>`<tr><td class="auto-mono">${esc(r.pattern)}</td><td>${esc(r.color)}</td>${del('color',r.id)}</tr>`).join(''):empty(3,'沒有上色的樣式。');
  }

  // ---------- 觸發（照 zMUD 的排版：左邊清單、右邊編輯） ----------
  // 清單排序同 Windows 檔案總管：不分大小寫、數字照大小比（trigger2 在 trigger10 前面）。
  // 有類別的排前面、依類別排，同類別再依句型排；沒有類別的排最後。只影響清單顯示，不改觸發的比對順序。
  const naturalCompare=new Intl.Collator('en',{numeric:true,sensitivity:'base'}).compare;
  function sortedTriggers(list){
    return [...list].sort((a,b)=>{
      if(!!a.cls!==!!b.cls)return a.cls?-1:1;
      return naturalCompare(a.cls,b.cls)||naturalCompare(a.pattern,b.pattern);
    });
  }
  let trgTab='main',selectedTrigger=null,trgEnabled=true,trgDirty=false;
  function renderTriggers(){
    $('#autoTriggersEnabled').checked=auto.state.triggersEnabled;
    const off=new Set(auto.state.disabledClasses);
    $('#trgClassOptions').innerHTML=auto.classes().map(c=>`<option value="${esc(c)}"></option>`).join('');
    const rows=sortedTriggers(auto.state.triggers).map(t=>{
      const classOff=t.cls&&off.has(t.cls);
      const on=t.enabled&&!classOff;
      return `<div class="trg-item${t.id===selectedTrigger?' selected':''}${on?'':' is-off'}" role="option" tabindex="0" data-trigger-id="${t.id}" aria-selected="${t.id===selectedTrigger}" title="${on?'已啟動':classOff?'類別已關閉':'已停用'}（雙擊切換）">
        <span class="trg-check">${on?'✔':''}</span><span class="trg-cls">${esc(t.cls)}</span><span class="trg-pat">${esc(t.pattern)}</span></div>`;
    }).join('');
    $('#autoTriggerList').innerHTML=`<div class="trg-item trg-list-head"><span></span><span>類別</span><span>句型</span></div>`+
      (rows||'<div class="auto-muted trg-empty">尚未設定任何觸發，按「新增」開始。</div>');
    renderTriggerExtras();
  }
  function renderTriggerExtras(){
    for(const b of dialog.querySelectorAll('[data-trg-tab]')){const on=b.dataset.trgTab===trgTab;b.classList.toggle('active',on);b.setAttribute('aria-selected',on?'true':'false');}
    for(const p of dialog.querySelectorAll('[data-trg-panel]'))p.hidden=p.dataset.trgPanel!==trgTab;
    const btn=$('#trgEnabledBtn');
    btn.classList.toggle('off',!trgEnabled);btn.setAttribute('aria-pressed',trgEnabled?'true':'false');
    btn.querySelector('b').textContent=trgEnabled?'已啟動':'已停用';
    const cls=$('#trgClass').value.trim();
    $('#trgClassState').textContent=cls?(auto.state.disabledClasses.includes(cls)?`類別「${cls}」目前關閉中，這一類的觸發都不會動作。`:`類別「${cls}」開啟中。`):'';
    $('#trgClassOn').disabled=$('#trgClassOff').disabled=!cls;
    $('#trgDelete').disabled=$('#trgCopy').disabled=!selectedTrigger;
    $('#trgState').textContent=selectedTrigger?(trgDirty?'有修改尚未儲存':''):(trgDirty?'新的觸發，按「確定」儲存':'新的觸發');
  }

  function fillTriggerForm(t){
    selectedTrigger=t?.id||null;
    $('#trgPattern').value=t?.pattern||'';
    $('#trgCommand').value=t?.command||'';
    $('#trgClass').value=t?.cls||'';
    trgEnabled=t?t.enabled:true;
    $('#trgCase').checked=!!t?.caseSensitive;
    $('#trgRegex').checked=!!t?.regex;$('#trgKindPattern').checked=!t?.regex;
    $('#trgPrompt').checked=!!t?.prompt;
    $('#trgError').textContent='';
    $('#trgTestResult').textContent='';
    trgDirty=false;
    lastStructure='';render();
  }

  function readTriggerForm(){
    return {id:selectedTrigger||undefined,pattern:$('#trgPattern').value.trim(),command:$('#trgCommand').value.trim(),
      cls:$('#trgClass').value.trim(),enabled:trgEnabled,caseSensitive:$('#trgCase').checked,
      regex:$('#trgRegex').checked,prompt:$('#trgPrompt').checked};
  }
  // 換到別的觸發前，有沒存的修改先問一下。
  function confirmLeave(){return !trgDirty||confirm('目前的觸發有修改還沒按「確定」，要放棄這些修改嗎？');}

  function fmt(sec){
    const s=Math.ceil(sec);
    return s>=60?`${Math.floor(s/60)}:${String(s%60).padStart(2,'0')}`:`${s} 秒`;
  }
  // ---------- 計時器（同觸發的排版；好幾組可以同時倒數） ----------
  let tmrTab='main',tmrDirty=false;
  function renderTimers(){
    const rows=auto.state.timers.map(t=>{
      const left=auto.remaining(t.id),running=left!==null;
      return `<div class="trg-item tmr-item${t.id===editingTimer?' selected':''}${running?' is-running':''}" role="option" tabindex="0" data-timer-id="${t.id}" aria-selected="${t.id===editingTimer}" title="雙擊${running?'停止':'開始'}倒數">
        <span class="trg-check" data-timer-dot="${t.id}">${running?'▶':''}</span><span class="trg-pat">${esc(t.name||t.command)}</span><span class="tmr-sec">${t.seconds} 秒${t.loop?'・循環':''}</span><span class="auto-left" data-timer-left="${t.id}">${running?fmt(left):'—'}</span></div>`;
    }).join('');
    $('#autoTimerList').innerHTML=`<div class="trg-item tmr-item trg-list-head"><span></span><span>名稱</span><span>倒數</span><span>剩餘</span></div>`+
      (rows||'<div class="auto-muted trg-empty">尚未設定任何計時器，按「新增」開始。</div>');
    renderTimerExtras();
  }
  function renderTimerExtras(){
    for(const b of dialog.querySelectorAll('[data-tmr-tab]')){const on=b.dataset.tmrTab===tmrTab;b.classList.toggle('active',on);b.setAttribute('aria-selected',on?'true':'false');}
    for(const p of dialog.querySelectorAll('[data-tmr-panel]'))p.hidden=p.dataset.tmrPanel!==tmrTab;
    const btn=$('#tmrRunBtn');
    const left=editingTimer?auto.remaining(editingTimer):null,running=left!==null;
    btn.classList.toggle('off',!running);btn.setAttribute('aria-pressed',running?'true':'false');
    btn.querySelector('b').textContent=running?`倒數中 ${fmt(left)}（按一下停止）`:'已停止（按一下開始）';
    btn.disabled=!editingTimer;
    $('#tmrRunNote').textContent=editingTimer?'':'新的計時器要先按「確定」儲存，才能開始倒數。';
    $('#tmrDelete').disabled=$('#tmrCopy').disabled=!editingTimer;
    $('#tmrState').textContent=editingTimer?(tmrDirty?'有修改尚未儲存（儲存後若正在倒數會從頭開始）':''):(tmrDirty?'新的計時器，按「確定」儲存':'新的計時器');
  }
  // 倒數中只更新剩餘秒數，不重建整張清單（避免打斷操作）。
  function refreshTimerCountdown(){
    for(const cell of dialog.querySelectorAll('[data-timer-left]')){
      const left=auto.remaining(cell.dataset.timerLeft);
      const text=left===null?'—':fmt(left);
      if(cell.textContent!==text)cell.textContent=text;
    }
    renderTimerExtras();
  }

  function fillTimerForm(t){
    editingTimer=t?.id||null;
    $('#tmrName').value=t?.name||'';
    $('#tmrSeconds').value=t?.seconds??'';
    $('#tmrFireAt').value=t?.fireAt??0;
    $('#tmrCommand').value=t?.command||'';
    $('#tmrLoop').checked=!!t?.loop;
    $('#tmrError').textContent='';
    tmrDirty=false;
    lastStructure='';render();
  }
  function confirmLeaveTimer(){return !tmrDirty||confirm('目前的計時器有修改還沒按「確定」，要放棄這些修改嗎？');}

  let lastStructure='';
  function render(){
    renderToggle();
    if(!dialog.open)return;
    // 結構（設定）有變才重建表格；只有倒數變化時只更新數字。
    const structure=JSON.stringify(auto.state)+'|'+[...auto.running.keys()].join(',');
    if(structure!==lastStructure){lastStructure=structure;renderTabs();renderTriggers();renderTimers();renderScript();}
    else refreshTimerCountdown();
  }
  auto.onChange(render);

  function open(which){
    tab=which||tab;lastStructure='';
    if(!dialog.open)dialog.showModal();
    render();
  }
  for(const [btn,which] of openButtons||[])btn?.addEventListener('click',()=>open(which));
  toggleButton?.addEventListener('click',e=>{
    e.preventDefault();
    auto.setPaused(!auto.state.paused);
    auto.notify(auto.state.paused?'自動功能已暫停：觸發與計時器都不會送出指令。':'自動功能已恢復。');
  });

  dialog.addEventListener('click',e=>{
    const t=e.target instanceof Element?e.target:null;if(!t)return;
    const b=t.closest('button');
    if(b?.dataset.autoTab){tab=b.dataset.autoTab;lastStructure='';render();return;}
    if(b?.dataset.trgTab){trgTab=b.dataset.trgTab;renderTriggerExtras();return;}
    const item=t.closest('[data-trigger-id]');
    if(item){
      const id=item.dataset.triggerId;
      if(id!==selectedTrigger&&confirmLeave())fillTriggerForm(auto.state.triggers.find(x=>x.id===id));
      return;
    }
    if(b?.dataset.scriptDel){
      const kind=b.dataset.scriptDel,key=b.dataset.key;
      if(kind==='alias')delete auto.state.aliases[key];
      else if(kind==='var')delete auto.state.vars[key];
      else if(kind==='gag')auto.state.gags=auto.state.gags.filter(r=>r.id!==key);
      else if(kind==='color')auto.state.colors=auto.state.colors.filter(r=>r.id!==key);
      auto.save();return;
    }
    if(b?.dataset.tmrTab){tmrTab=b.dataset.tmrTab;renderTimerExtras();return;}
    const titem=t.closest('[data-timer-id]');
    if(titem){
      const id=titem.dataset.timerId;
      if(id!==editingTimer&&confirmLeaveTimer())fillTimerForm(auto.state.timers.find(x=>x.id===id));
      return;
    }
  });
  // 雙擊清單裡的觸發：切換開關（直接存檔）；雙擊計時器：開始／停止倒數。
  dialog.addEventListener('dblclick',e=>{
    const titem=e.target instanceof Element?e.target.closest('[data-timer-id]'):null;
    if(titem){const id=titem.dataset.timerId;if(auto.running.has(id))auto.stopTimer(id);else auto.startTimer(id);return;}
    const item=e.target instanceof Element?e.target.closest('[data-trigger-id]'):null;if(!item)return;
    const x=auto.state.triggers.find(y=>y.id===item.dataset.triggerId);if(!x)return;
    x.enabled=!x.enabled;
    if(x.id===selectedTrigger)trgEnabled=x.enabled;
    auto.save();
  });
  dialog.addEventListener('keydown',e=>{
    const item=e.target instanceof Element?e.target.closest('[data-trigger-id],[data-timer-id]'):null;
    if(item&&(e.key==='Enter'||e.key===' ')){e.preventDefault();item.click();}
  });
  dialog.addEventListener('change',e=>{
    const t=e.target;
    if(t.id==='autoTriggersEnabled')auto.setTriggersEnabled(t.checked);
    else if(t.closest?.('#autoTriggerPane')&&t.id!=='trgSample'){trgDirty=true;renderTriggerExtras();}
    else if(t.closest?.('#autoTimerPane')){tmrDirty=true;renderTimerExtras();}
  });
  dialog.addEventListener('input',e=>{
    const t=e.target;
    if(t.closest?.('#autoTriggerPane')&&['trgPattern','trgCommand','trgClass'].includes(t.id)){trgDirty=true;renderTriggerExtras();}
    if(t.closest?.('#autoTimerPane')&&t.id&&t.id.startsWith('tmr')){tmrDirty=true;renderTimerExtras();}
  });

  $('#trgSave').addEventListener('click',()=>{
    const x=readTriggerForm();
    if(!x.pattern){trgTab='main';renderTriggerExtras();$('#trgError').textContent='請輸入句型。';return;}
    try{const saved=auto.upsertTrigger(x);fillTriggerForm(saved);}
    catch(err){$('#trgError').textContent='句型有誤：'+err.message;}
  });
  $('#trgCancel').addEventListener('click',()=>fillTriggerForm(auto.state.triggers.find(x=>x.id===selectedTrigger)||null));
  $('#trgNew').addEventListener('click',()=>{if(confirmLeave()){trgTab='main';fillTriggerForm(null);$('#trgPattern').focus();}});
  $('#trgCopy').addEventListener('click',()=>{
    const x=auto.state.triggers.find(y=>y.id===selectedTrigger);if(!x||!confirmLeave())return;
    fillTriggerForm({...x,id:undefined});trgDirty=true;renderTriggerExtras();$('#trgPattern').focus();
  });
  $('#trgDelete').addEventListener('click',()=>{
    const x=auto.state.triggers.find(y=>y.id===selectedTrigger);
    if(x&&confirm(`刪除觸發「${x.pattern}」？`)){auto.removeTrigger(x.id);fillTriggerForm(null);}
  });
  $('#trgEnabledBtn').addEventListener('click',()=>{trgEnabled=!trgEnabled;trgDirty=true;renderTriggerExtras();});
  $('#trgClassOn').addEventListener('click',()=>{const c=$('#trgClass').value.trim();if(c)auto.toggleClass(c,true);});
  $('#trgClassOff').addEventListener('click',()=>{const c=$('#trgClass').value.trim();if(c)auto.toggleClass(c,false);});
  $('#trgTest').addEventListener('click',()=>{
    const x=readTriggerForm(),sample=$('#trgSample').value;
    try{
      const m=compilePattern(x.pattern,x).exec(sample);
      $('#trgTestResult').textContent=m
        ? `符合。會送出：${expandCommand(x.command,m)||'（沒有指令）'}${m.length>1?'　擷取：'+m.slice(1).map((v,i)=>`%${i+1}=${v}`).join('，'):''}`
        : '不符合。';
    }catch(err){$('#trgTestResult').textContent='樣式有誤：'+err.message;}
  });

  $('#tmrSave').addEventListener('click',()=>{
    const err=m=>{$('#tmrError').textContent=m;};
    const seconds=Number($('#tmrSeconds').value),fireAt=Number($('#tmrFireAt').value||0);
    if(!Number.isFinite(seconds)||seconds<1){tmrTab='main';renderTimerExtras();err('倒數秒數至少 1 秒。');return;}
    if(!Number.isFinite(fireAt)||fireAt<0){tmrTab='options';renderTimerExtras();err('「剩幾秒時執行」不能小於 0。');return;}
    if(!$('#tmrCommand').value.trim()){tmrTab='main';renderTimerExtras();err('請輸入要執行的指令。');return;}
    try{
      const saved=auto.upsertTimer({id:editingTimer||undefined,name:$('#tmrName').value.trim(),seconds,fireAt,
        command:$('#tmrCommand').value.trim(),loop:$('#tmrLoop').checked});
      fillTimerForm(saved);
    }catch(e){err(e.message);}
  });
  $('#tmrCancel').addEventListener('click',()=>fillTimerForm(auto.state.timers.find(x=>x.id===editingTimer)||null));
  $('#tmrNew').addEventListener('click',()=>{if(confirmLeaveTimer()){tmrTab='main';fillTimerForm(null);$('#tmrName').focus();}});
  $('#tmrCopy').addEventListener('click',()=>{
    const x=auto.state.timers.find(y=>y.id===editingTimer);if(!x||!confirmLeaveTimer())return;
    fillTimerForm({...x,id:undefined});tmrDirty=true;renderTimerExtras();$('#tmrName').focus();
  });
  $('#tmrDelete').addEventListener('click',()=>{
    const x=auto.state.timers.find(y=>y.id===editingTimer);
    if(x&&confirm(`刪除計時器「${x.name||x.command}」？`)){auto.removeTimer(x.id);fillTimerForm(null);}
  });
  $('#tmrRunBtn').addEventListener('click',()=>{
    if(!editingTimer)return;
    if(auto.running.has(editingTimer))auto.stopTimer(editingTimer);else auto.startTimer(editingTimer);
  });

  $('#autoExport').addEventListener('click',()=>{
    const a=document.createElement('a');
    const stamp=new Date().toISOString().slice(0,10);
    a.href=URL.createObjectURL(new Blob([auto.exportJSON()],{type:'application/json'}));
    a.download=`es2-automation-${stamp}.json`;
    document.body.appendChild(a);a.click();
    setTimeout(()=>{a.remove();URL.revokeObjectURL(a.href);},1000);
  });
  const fileInput=$('#autoImportFile');
  $('#autoImport').addEventListener('click',()=>fileInput.click());
  fileInput.addEventListener('change',async()=>{
    const f=fileInput.files?.[0];fileInput.value='';
    if(!f)return;
    if(!confirm('匯入會取代目前所有的觸發、計時器、縮寫與變數設定，確定嗎？'))return;
    try{
      const r=auto.importJSON(await f.text());
      auto.notify(`已匯入 ${r.triggers} 個觸發、${r.timers} 個計時器、${r.aliases} 個縮寫。`);
    }catch(err){alert('匯入失敗：'+err.message);}
  });

  fillTriggerForm(null);fillTimerForm(null);renderToggle();
  return {open};
}
