// zMUD 風格的指令語言：大括號區塊、條件運算式、變數與函數、快速移動。
// 由 automation.js 使用（設計文件：docs/WEB_ZMUD_COMMANDS.md）。
// 運算式只由這裡自己逐字解析，不會把文字當成程式執行。

// ---------- 拆指令 ----------
// 以 ; 拆成一個個指令，但大括號 {…}、小括號 (…)、雙引號 "…" 裡面的 ; 不拆。
export function splitStatements(text){
  const out=[];let cur='',brace=0,paren=0,quote=false;
  const s=String(text??'');
  for(let i=0;i<s.length;i++){
    const c=s[i];
    if(c==='~'&&i+1<s.length){cur+=c+s[++i];continue;}
    if(quote){cur+=c;if(c==='"')quote=false;continue;}
    if(c==='"'&&(brace||paren)){quote=true;cur+=c;continue;}
    if(c==='{')brace++;
    else if(c==='}'&&brace)brace--;
    else if(c==='('&&brace===0)paren++;
    else if(c===')'&&brace===0&&paren)paren--;
    if(c===';'&&!brace&&!paren){out.push(cur.trim());cur='';continue;}
    cur+=c;
  }
  out.push(cur.trim());
  return out.filter(Boolean);
}

// 從 s 的 i 位置讀一個參數：{區塊}、(運算式)、"字串" 或一個字。回傳 {value,raw,kind,end}。
export function readArg(s,i){
  while(i<s.length&&/\s/.test(s[i]))i++;
  if(i>=s.length)return null;
  const open=s[i];
  if(open==='{'||open==='('){
    const close=open==='{'?'}':')';let depth=0,quote=false;
    for(let j=i;j<s.length;j++){
      const c=s[j];
      if(c==='~'){j++;continue;}
      if(quote){if(c==='"')quote=false;continue;}
      if(c==='"'){quote=true;continue;}
      if(c===open)depth++;
      else if(c===close&&--depth===0)return {value:s.slice(i+1,j),raw:s.slice(i,j+1),kind:open,end:j+1};
    }
    return {value:s.slice(i+1),raw:s.slice(i),kind:open,end:s.length}; // 少了右括號：讀到底
  }
  if(open==='"'){
    const j=s.indexOf('"',i+1);
    const end=j<0?s.length:j;
    return {value:s.slice(i+1,end),raw:s.slice(i,end+1),kind:'"',end:Math.min(s.length,end+1)};
  }
  let j=i;while(j<s.length&&!/\s/.test(s[j]))j++;
  return {value:s.slice(i,j),raw:s.slice(i,j),kind:'w',end:j};
}
// 讀出全部參數。
export function readArgs(s){
  const out=[];let i=0,a;
  s=String(s??'');
  while((a=readArg(s,i))){out.push(a);i=a.end;}
  return out;
}

// ---------- 快速移動 ----------
// .3n2e → n;n;n;e;e    斜角方向用括號：.2(ne)s → ne;ne;s
const WALK_DIRS={n:'n',s:'s',e:'e',w:'w',u:'u',d:'d'};
export function expandSpeedwalk(text){
  const m=String(text).trim().match(/^\.((?:\d*(?:[nsewud]|\([a-z]+\)))+)$/i);
  if(!m)return null;
  const out=[];const re=/(\d*)(?:([nsewud])|\(([a-z]+)\))/gi;let x;
  while((x=re.exec(m[1]))){
    const n=Math.min(99,x[1]?Number(x[1]):1);
    const dir=x[2]?WALK_DIRS[x[2].toLowerCase()]:x[3].toLowerCase();
    for(let i=0;i<n;i++)out.push(dir);
  }
  return out;
}

// ---------- 值 ----------
const NUM_RE=/^\s*-?\d+(?:\.\d+)?\s*$/;
export const isNum=v=>typeof v==='number'||(typeof v==='string'&&NUM_RE.test(v));
const toNum=v=>typeof v==='number'?v:(isNum(v)?Number(v):0);
const fmt=v=>typeof v==='number'?(Number.isInteger(v)?String(v):String(Math.round(v*1000)/1000)):String(v??'');
export const truthy=v=>typeof v==='number'?v!==0:(v!==''&&v!=='0'&&v!==undefined&&v!==null);

const VAR_RE=/@([\p{L}\p{N}_]+)/gu;

// ---------- 函數 ----------
const pad2=n=>String(n).padStart(2,'0');
function callFunction(name,args,ctx){
  const n=name.toLowerCase();
  const ev=a=>evaluate(a,ctx);
  switch(n){
    case 'len':return [...String(ev(args[0]??''))].length;
    case 'random':{
      if(args.length>=2){const lo=Math.trunc(toNum(ev(args[0]))),hi=Math.trunc(toNum(ev(args[1])));return lo+Math.floor(Math.random()*(Math.max(lo,hi)-lo+1));}
      const hi=Math.trunc(toNum(ev(args[0]??'0')));return Math.floor(Math.random()*(Math.max(0,hi)+1));
    }
    case 'time':{const d=new Date();return `${pad2(d.getHours())}:${pad2(d.getMinutes())}:${pad2(d.getSeconds())}`;}
    case 'left':{const s=[...String(ev(args[0]??''))];return s.slice(0,Math.max(0,Math.trunc(toNum(ev(args[1]??'0'))))).join('');}
    case 'right':{const s=[...String(ev(args[0]??''))],k=Math.max(0,Math.trunc(toNum(ev(args[1]??'0'))));return k?s.slice(-k).join(''):'';}
    case 'pos':{
      // %pos(要找的字, 字串)：從 1 開始算的位置，找不到是 0。
      const sub=String(ev(args[0]??'')),s=String(ev(args[1]??''));
      const i=s.indexOf(sub);return i<0?0:[...s.slice(0,i)].length+1;
    }
    case 'eval':return ev(args[0]??'');
    case 'if':return truthy(ev(args[0]??''))?ev(args[1]??''):ev(args[2]??'');
  }
  return undefined;
}
export const FUNCTIONS=['len','random','time','left','right','pos','eval','if'];

// 函數的參數以頂層的逗號分開。
function splitArgs(s){
  const out=[];let cur='',depth=0,brace=0,quote=false;
  for(const c of s){
    if(quote){cur+=c;if(c==='"')quote=false;continue;}
    if(c==='"'){quote=true;cur+=c;continue;}
    if(c==='(')depth++;else if(c===')')depth--;
    else if(c==='{')brace++;else if(c==='}')brace--;
    if(c===','&&!depth&&!brace){out.push(cur.trim());cur='';continue;}
    cur+=c;
  }
  if(cur.trim()||out.length)out.push(cur.trim());
  return out;
}

// ---------- 運算式 ----------
// 值：數字、"字串"、沒加引號的文字（連在一起的字當成一段文字）、@變數、%函數(…)
// 運算：! 不是；* / % 乘除餘（除法取整數）；+ -；= == != <> < > <= >= 比較；& && 而且；| || 或者；括號分組。
function tokenize(src,ctx){
  const t=[];const s=String(src??'');let i=0;
  const push=(type,value)=>t.push({type,value});
  while(i<s.length){
    const c=s[i];
    if(/\s/.test(c)){i++;continue;}
    const two=s.slice(i,i+2);
    if(['<=','>=','!=','<>','==','&&','||'].includes(two)){push('op',two==='<>'?'!=':two==='=='?'=':two==='&&'?'&':two==='||'?'|':two);i+=2;continue;}
    if('+-*/%<>=&|!()'.includes(c)){
      // %函數(…) 與 %數字（觸發沒有展開到的擷取）
      if(c==='%'){
        const m=s.slice(i).match(/^%([a-z]+)(\()?/i);
        if(m&&FUNCTIONS.includes(m[1].toLowerCase())){
          if(m[2]){
            let depth=0,j=i+m[0].length-1,quote=false;
            for(;j<s.length;j++){const d=s[j];if(quote){if(d==='"')quote=false;continue;}if(d==='"'){quote=true;continue;}if(d==='(')depth++;else if(d===')'&&--depth===0)break;}
            push('val',callFunction(m[1],splitArgs(s.slice(i+m[0].length,j)),ctx));i=j+1;continue;
          }
          push('val',callFunction(m[1],[],ctx));i+=m[0].length;continue;
        }
      }
      push('op',c);i++;continue;
    }
    if(c==='"'){const j=s.indexOf('"',i+1);const end=j<0?s.length:j;push('val',s.slice(i+1,end));i=end+1;continue;}
    if(c==='@'){
      const m=s.slice(i).match(/^@([\p{L}\p{N}_]+)/u);
      if(m){push('val',ctx.getVar(m[1]));i+=m[0].length;continue;}
    }
    if(/[0-9.]/.test(c)){const m=s.slice(i).match(/^\d+(?:\.\d+)?/);if(m){push('val',Number(m[0]));i+=m[0].length;continue;}}
    // 沒加引號的文字：到下一個運算符號為止
    let j=i;while(j<s.length&&!'+-*/%<>=&|!()"@'.includes(s[j]))j++;
    if(j===i)j++;
    push('word',s.slice(i,j).trim());i=j;
  }
  // 相鄰的文字併成一段（例如觸發帶入「測試 玩家」）
  const out=[];
  for(const x of t){
    const last=out[out.length-1];
    if(x.type==='word'&&last&&last.type==='word'){last.value+=' '+x.value;continue;}
    out.push(x);
  }
  return out.map(x=>x.type==='word'?{type:'val',value:isNum(x.value)?Number(x.value):x.value}:x);
}

export function evaluate(src,ctx){
  const t=tokenize(src,ctx);let i=0;
  const peek=()=>t[i],isOp=v=>t[i]&&t[i].type==='op'&&t[i].value===v;
  function primary(){
    const x=t[i];
    if(!x)return '';
    if(x.type==='op'&&x.value==='('){i++;const v=or();if(isOp(')'))i++;return v;}
    if(x.type==='op'&&x.value==='!'){i++;return truthy(primary())?0:1;}
    if(x.type==='op'&&x.value==='-'){i++;return -toNum(primary());}
    if(x.type==='op'&&x.value==='+'){i++;return toNum(primary());}
    if(x.type==='val'){i++;return x.value;}
    i++;return '';
  }
  function mul(){
    let v=primary();
    while(peek()&&peek().type==='op'&&'*/%'.includes(peek().value)){
      const op=t[i++].value,r=toNum(primary()),l=toNum(v);
      v=op==='*'?l*r:r===0?0:op==='/'?Math.trunc(l/r):l%r;
    }
    return v;
  }
  function add(){
    let v=mul();
    while(peek()&&peek().type==='op'&&(peek().value==='+'||peek().value==='-')){
      const op=t[i++].value,r=mul();
      v=op==='+'?toNum(v)+toNum(r):toNum(v)-toNum(r);
    }
    return v;
  }
  function cmp(){
    let v=add();
    while(peek()&&peek().type==='op'&&['=','!=','<','>','<=','>='].includes(peek().value)){
      const op=t[i++].value,r=add();
      const both=isNum(v)&&isNum(r);
      const a=both?toNum(v):fmt(v),b=both?toNum(r):fmt(r);
      v=(op==='='?a===b:op==='!='?a!==b:op==='<'?a<b:op==='>'?a>b:op==='<='?a<=b:a>=b)?1:0;
    }
    return v;
  }
  function and(){let v=cmp();while(isOp('&')){i++;const r=cmp();v=truthy(v)&&truthy(r)?1:0;}return v;}
  function or(){let v=and();while(isOp('|')){i++;const r=and();v=truthy(v)||truthy(r)?1:0;}return v;}
  const v=or();
  return typeof v==='number'?v:v;
}
export const formatValue=fmt;

// ---------- 展開文字裡的 @變數 與 %函數(…) ----------
// ~@ 寫出 @ 本身。沒有這個變數時保留原文（避免把信箱之類的文字吃掉）。
export function expandText(text,ctx){
  let s=String(text??'');
  // %函數
  let out='',i=0;
  while(i<s.length){
    if(s[i]==='~'&&i+1<s.length){out+=s[i]+s[i+1];i+=2;continue;}
    if(s[i]==='%'){
      const m=s.slice(i).match(/^%([a-z]+)(\()?/i);
      if(m&&FUNCTIONS.includes(m[1].toLowerCase())){
        if(m[2]){
          let depth=0,j=i+m[0].length-1,quote=false;
          for(;j<s.length;j++){const d=s[j];if(quote){if(d==='"')quote=false;continue;}if(d==='"'){quote=true;continue;}if(d==='(')depth++;else if(d===')'&&--depth===0)break;}
          out+=fmt(callFunction(m[1],splitArgs(s.slice(i+m[0].length,j)),ctx));i=j+1;continue;
        }
        if(m[1].toLowerCase()==='time'){out+=fmt(callFunction('time',[],ctx));i+=m[0].length;continue;}
      }
    }
    out+=s[i++];
  }
  // @變數
  out=out.replace(/(~?)@([\p{L}\p{N}_]+)/gu,(all,tilde,name)=>{
    if(tilde)return '@'+name;
    return ctx.hasVar(name)?fmt(ctx.getVar(name)):all;
  });
  return out.replace(/~([;{}~@%])/g,'$1');
}
export {VAR_RE};
