import {SessionRecorder} from './session_recorder.js';
import {PlayerStateObserver} from './player_state.js';
import {Automation,setupAutomationUI} from './automation.js';
import {setupChatWindow} from './chat_window.js';
import {setupFloatWindow,setupSideSplitter,bringToFront} from './float_window.js';
import {setupTextColor} from './text_color.js';
import {bgTimeout,clearBg,keepPageAlive} from './bg_timer.js';

const sessionRecorder = new SessionRecorder();
const observer = new PlayerStateObserver();
const term = document.querySelector('#terminal');
const input = document.querySelector('#command');
const conn = document.querySelector('#conn');
const gameApp = document.querySelector('#gameApp');

let ws = null;
let started = false;
// 指令紀錄：只保存超過 3 個字元的指令、最新 100 個，存在瀏覽器（localStorage），
// 重新整理或下次開啟仍在。重複的指令會移到最新，不會重複佔位。
const HISTORY_KEY = 'es2-cmd-history';
const HISTORY_MAX = 100;
const HISTORY_MIN_LEN = 4;
let history = [];
try {
  const saved = JSON.parse(localStorage.getItem(HISTORY_KEY) || '[]');
  if (Array.isArray(saved)) history = saved.filter(x => typeof x === 'string').slice(-HISTORY_MAX);
} catch (_) {}
let hi = history.length;
// 上下鍵翻指令時，以按下第一次上鍵當時輸入欄的文字為開頭來找（例如輸入 c 再按上，
// 只會翻到 c 開頭的指令）。開始打字或送出後重設。
let historyPrefix = null;
function rememberCommand(c){
  c = String(c ?? '').trim();
  if (c.length < HISTORY_MIN_LEN) return;
  history = history.filter(x => x !== c);
  history.push(c);
  if (history.length > HISTORY_MAX) history = history.slice(-HISTORY_MAX);
  try { localStorage.setItem(HISTORY_KEY, JSON.stringify(history)); } catch (_) {}
}
let retry = 0;
// 斷線重連：等待時間逐次拉長（1、2、4、8、16、30 秒），再加一點隨機延遲，避免伺服器出狀況時
// 所有玩家每秒同時重連、把伺服器壓垮。連線穩定 30 秒以上才把等待時間歸零。
let retryResetTimer = null;
const RECONNECT_MAX_MS = 30000;
const RECONNECT_STABLE_MS = 30000;
function reconnectDelay(){
  const base=Math.min(RECONNECT_MAX_MS,1000*2**Math.min(retry++,5));
  return Math.round(base+Math.random()*base*0.3);
}
let tail = '';
let graph = {nodes:[], edges:[]};
let nodeById = new Map();
let labelIndex = new Map();
let adjacency = new Map();
let currentRoomId = null;
let mapBootstrapRetryTimer=null;
let lastMissingMapRoomId='';
let hudTimer = null;
let hudPollInFlight = false;
let lastUserCommandAt = 0;
let hudKickTimer = null;
let hudResponseTimer = null;
let hudPollBuffer = '';
let hudPollStartedAt = 0;
let hudFallbackBuffer = '';
let pendingUserCommands = [];
let lastReceiveAt = 0;
let welcomeStyled = true;
let mapRefreshPending = false;
let hudBootstrapInFlight = false;
let hudBootstrapAttempts = 0;
const HUD_POLL_MS = 800;
// 背景狀態查詢的頻率：戰鬥中每 0.8 秒；平常每 2.5 秒；玩家剛下指令的 3 秒內，
// 指令的回應一到就立刻更新（吃喝、裝備、移動後狀態欄馬上反映）。分頁在背景時完全暫停。
const HUD_IDLE_POLL_MS = 2500;
const HUD_AFTER_COMMAND_MS = 3000;
let lastHudPollAt = 0;
const HUD_USER_GRACE_MS = 350;
const HUD_KICK_DELAY_MS = 20;
const HUD_POLL_TIMEOUT_MS = 1200;
const HUD_ACTION_FLUSH_MS = 180;

const esc = s => String(s).replace(/[&<>\"]/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','\"':'&quot;'}[c]));
const baseColors = {
  30:'#707770',31:'#ff7474',32:'#65df8b',33:'#efd06b',34:'#76a9ff',35:'#d98df0',36:'#69dbe5',37:'var(--term-fg)',
  90:'#8e978f',91:'#ff9999',92:'#8be9a6',93:'#f4dc8c',94:'#9bc0ff',95:'#e6abf5',96:'#93edf3',97:'#ffffff'
};
const bgColors = {40:'#090b0a',41:'#532020',42:'#1d4d2e',43:'#4e4219',44:'#1c3150',45:'#47244d',46:'#17464a',47:'#d7ddd9'};


const contextTitle=document.getElementById('contextTitle');
const contextBody=document.getElementById('contextBody');
let contextState={room:'',people:[],items:[],combat:null,inventory:[]};
let contextRoomScan=false;
let chatParseBuffer='';
// 聊天視窗（左側「聊天」開關）。頻道：閒聊、謠言、密語（tell/reply）、對談（say），
// 其他頻道（系統、巫師、隊伍、幫派…）只在「全部」裡出現。
const chatWindow=setupChatWindow({
  root:document.getElementById('chatWindow'),
  openButton:document.querySelector('[data-ui-chat]'),
});
document.getElementById('chatWindow')?.addEventListener('pointerdown',e=>bringToFront(e.currentTarget));
// 最右欄是空白區域，用來擺地圖、角色狀態等浮動視窗；訊息欄與它之間的分隔線可左右拖曳。
// 訊息欄最窄 = score 最寬的一行（72 個半形字寬）× 125%，加上左右留白；跟著文字大小設定變動（同 styles.css 的 --console-min）。
const SCORE_COLUMNS=72;
const consoleMinWidth=()=>(parseFloat(getComputedStyle(term).fontSize)||15)*SCORE_COLUMNS/2*1.25+40;
const sideSplitter=setupSideSplitter({shell:document.querySelector('.game-shell'),splitter:document.getElementById('sideSplitter'),storageKey:'es2-side-width-v1',minCenter:consoleMinWidth});
// 第一次打開時，地圖與角色狀態放在最右欄（最右欄被拉到很窄時改靠畫面右邊）。
function sideArea(){
  const r=document.querySelector('.game-side')?.getBoundingClientRect();
  return r&&r.width>=200?{left:r.left+12,width:r.width-24}:{left:window.innerWidth-412,width:400};
}
setupFloatWindow({
  root:document.getElementById('mapWindow'),
  openButton:document.querySelector('[data-ui-map]'),
  storageKey:'es2-map-window-v1',resizable:true,defaultOpen:true,
  defaultGeometry:()=>{const a=sideArea();return {left:a.left,top:12,width:Math.max(300,a.width),height:Math.min(470,window.innerHeight-280)};},
});
setupFloatWindow({
  root:document.getElementById('statusWindow'),
  openButton:document.querySelector('[data-ui-status]'),
  storageKey:'es2-status-window-v1',defaultOpen:true,
  defaultGeometry:()=>{
    const a=sideArea(),m=document.getElementById('mapWindow');
    const top=m&&!m.hidden?m.getBoundingClientRect().bottom+10:12;
    return {left:a.left,top:Math.min(top,window.innerHeight-240)};
  },
});
function extractChatMessage(plain){
  let text=String(plain||'').trim();
  if(!text)return null;

  // Channel output can arrive while the telnet prompt is still on the same
  // physical line.  Do not require the CHAT/TELL marker to be column zero.
  const chatAt=text.indexOf('【閒聊】');
  if(chatAt>=0)return {kind:'chat',text:text.slice(chatAt).trim()};

  // Remove only browser/telnet prompt debris.  Incoming tells may otherwise be
  // prefixed by the prompt and were previously missed because the parser used
  // start-anchored regular expressions.
  text=text.replace(/^(?:[>＞]\s*)+/,'').trim();
  if(!text)return null;

  if(/^你告訴.+[：﹕:]/.test(text))return {kind:'tell',text};
  if(/^你回答.+[：﹕:]/.test(text))return {kind:'tell',text};
  if(/告訴你[：﹕:]/.test(text))return {kind:'tell',text};
  if(/回答你[：﹕:]/.test(text))return {kind:'tell',text};
  if(/^【謠言】/.test(text))return {kind:'rumor',text};
  if(/^[^\s【】：﹕:]{1,20}說道[：﹕:]/.test(text))return {kind:'say',text};
  // 其他頻道：【系統】【巫師】【東方故事】【隊伍】【幫派名】… 只出現在「全部」。
  if(/^【[^】\s]{1,8}】\S/.test(text))return {kind:'other',text};
  return null;
}
function captureChatMessages(raw){
  chatParseBuffer+=String(raw);
  const parts=chatParseBuffer.split(/\r\n|\n|\r/);
  chatParseBuffer=parts.pop()||'';
  for(const line of parts){
    const plain=cleanText(line).trim();
    if(!plain)continue;
    const hit=extractChatMessage(plain);
    if(!hit)continue;
    // Record normalized text rather than the whole transport line, otherwise a
    // leading command prompt becomes part of the chat history. Each channel has
    // its own color in the chat window, so ANSI preservation is unnecessary here.
    chatWindow.add(hit.kind,hit.text);
  }
}
function escContext(s){return String(s).replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));}
function combatStat(label,cur,max,sideKey){
  const pct=max>0?Math.max(0,Math.min(100,cur*100/max)):0;
  const segments=10;
  const lit=Math.max(0,Math.min(segments,Math.ceil(pct/100*segments)));
  const blocks=Array.from({length:segments},(_,i)=>`<i class="${i<lit?'on':''}"></i>`).join('');
  return `<div class="context-combat-stat ${sideKey}"><div class="context-combat-stat-head"><span>${label}</span></div><div class="context-combat-segments" aria-label="${label} ${lit}/${segments}格">${blocks}</div></div>`;
}
function combatCard(side,sideKey){
  if(!side)return `<div class="context-combat-card ${sideKey} context-combat-empty"></div>`;
  return `<div class="context-combat-card ${sideKey}"><strong>${escContext(side.name)}</strong>${combatStat('形體',side.hp,side.hpmax,sideKey)}${combatStat('精',side.gin,side.ginmax,sideKey)}${combatStat('氣',side.kee,side.keemax,sideKey)}${combatStat('神',side.sen,side.senmax,sideKey)}</div>`;
}
function itemActionLabel(action,equipped){
  if(equipped)return '已裝備';
  return ({eat:'吃',drink:'飲用',study:'研讀'})[action]||'';
}
function inventoryPanel(){
  const items=(contextState.inventory||[]).filter(it=>['eat','drink','study'].includes(it.action));
  const rows=items.slice(0,24).map(it=>{
    const label=itemActionLabel(it.action,it.equipped);
    const button=label?`<button type="button" class="context-item-action" data-command="${escContext(it.command)}">${label}</button>`:'';
    return `<div class="context-inventory-row"><span class="context-inventory-name">${escContext(it.name)}</span>${button}</div>`;
  });
  return `<div class="context-inventory"><div class="context-section-title">可使用道具</div>${rows.length?rows.join(''):'<div class="context-empty">目前沒有可直接使用的道具。</div>'}</div>`;
}
function bindCommandButtons(root){
  if(!root||root.dataset.commandDelegated==='1')return;
  root.dataset.commandDelegated='1';
  root.addEventListener('click',e=>{
    const btn=e.target.closest('.context-item-action[data-command]');
    if(!btn||!root.contains(btn))return;
    e.preventDefault();
    e.stopPropagation();
    const command=btn.dataset.command||'';
    if(!command)return;
    // Stable delegated handler: renderContext() replaces innerHTML frequently.
    // innerHTML frequently, so listen on the persistent panel instead of
    // relying on handlers attached to transient button nodes.
    send(command);
    if(input){input.value='';repeatArmed=false;input.focus();}
  });
}
function bindContextActions(){bindCommandButtons(contextBody);}
let lastContextHtml=null;
function renderContext(){
  if(!contextBody)return;
  let primary='';
  if(contextState.combat?.target){
    contextTitle.textContent='戰鬥狀態';
    primary=`<div class="context-combat-grid">${combatCard(contextState.combat.self,'is-self')}<div class="context-vs">VS</div>${combatCard(contextState.combat.target,'is-target')}</div>`;
  }else{
    contextTitle.textContent='可使用道具';
    primary='';
  }
  // HUD 每 0.8 秒回報一次，內容多半沒變；相同就不重建 DOM，避免長時間掛機一直重排版。
  const html=primary+inventoryPanel();
  if(html!==lastContextHtml){contextBody.innerHTML=html;lastContextHtml=html;}
  bindContextActions();
}
function observeContext(raw){
  // Context parsing uses plain text only. ANSI/control sequences remain available to the terminal renderer,
  // but are stripped here and non-room command output is never scanned as room entities.
  const text=cleanText(String(raw));
  const lines=text.split(/\r?\n/);
  for(const rawLine of lines){
    const line=rawLine.trim();
    if(!line)continue;
    if(line.startsWith('@@WEBHUD|'))continue;
    if(/^>\s*$/.test(line)){contextRoomScan=false;continue;}
    const m=line.match(/^([^<>]{1,24})\s+-\s*$/);
    if(m&&!/^(ES2|HTTP|WebMUD)/i.test(m[1])){
      contextState={...contextState,room:m[1].trim(),people:[],items:[]};
      contextRoomScan=true;
      continue;
    }
    if(!contextRoomScan)continue;
    // Only entity lines belonging to the active room description are eligible.
    const e=line.match(/^(.{1,40})\s+\(([^()]{1,40})\)\s*$/);
    if(e){
      const label=e[1].trim();
      if(/箱|劍|刀|衣|帽|瓶|石|鍋|包|棍|杖|槍|斧|鞋|甲|戒|符|藥|錢|金|杯|碗|壺/.test(label)){if(!contextState.items.includes(label))contextState.items.push(label);}
      else if(!contextState.people.includes(label))contextState.people.push(label);
    }
  }
  renderContext();
}
function parseWebHud(raw){
  for(const line of cleanText(String(raw)).split(/\r?\n/)){
    if(!line.startsWith('@@WEBHUD|'))continue;
    const a=line.split('|');
    if(a[1]==='ROOM'&&a.length>=3){beginRuntimeRoomSnapshot(a[2],a[3]||a[2]);continue;}
    if(a[1]==='MAPMETA'&&a.length>=3){setRoomMapMeta(a[2],a[3]||'',a[4]||'',a[5]||'');continue;}
    if(a[1]==='EXIT'&&a.length>=5){mergeRuntimeExit(a[2],a[3],a[4],a[5]||'');continue;}
    if(a[1]==='END'){finalizeRuntimeRoomSnapshot();continue;}
    if(a[1]==='INPUT'){serverInputWait=true;continue;}
    if(a[1]==='READY'){serverInputWait=false;continue;}
    if(a[1]==='INVBEGIN'){contextState.inventory=[];continue;}
    if(a[1]==='ITEM'&&a.length>=7){
      contextState.inventory.push({name:a[2],id:a[3],action:a[4],command:a[5],equipped:a[6]==='1'});
      continue;
    }
    if(a[1]==='INVEND'){renderContext();continue;}
    if(a[1]==='NONE'){contextState.combat=null;renderContext();continue;}
    if(a.length<12)continue;
    const side={name:a[2],hp:+a[4],hpmax:+a[5],gin:+a[6],ginmax:+a[7],kee:+a[8],keemax:+a[9],sen:+a[10],senmax:+a[11]};
    if(a[1]==='SELF'){
      // The silent LPC HUD feed is also the browser side-panel source, so the
      // role status no longer waits for the player to type hp/score manually.
      // a[19..22] 是最大值（score 的分母）；舊版伺服器沒有這四欄時退回用有效上限。
      const mx=i=>a.length>=23?+a[i]:0;
      observer.state.vitals.hp={current:side.hp,limit:mx(19)||side.hpmax,effective:side.hpmax,limitKind:'maximum'};
      observer.state.vitals.jing={current:side.gin,limit:mx(20)||side.ginmax,effective:side.ginmax,limitKind:'maximum'};
      observer.state.vitals.qi={current:side.kee,limit:mx(21)||side.keemax,effective:side.keemax,limitKind:'maximum'};
      observer.state.vitals.shen={current:side.sen,limit:mx(22)||side.senmax,effective:side.senmax,limitKind:'maximum'};
      if(a.length>=19){
        observer.state.score.level=+a[12];
        observer.state.vitals.food={current:+a[13],limit:+a[14],limitKind:'maximum'};
        observer.state.vitals.water={current:+a[15],limit:+a[16],limitKind:'maximum'};
        observer.state.vitals.fatigue={current:+a[17],limit:+a[18],limitKind:'maximum'};
      }
      renderObserved();
    }
    contextState.combat ||= {self:null,target:null};
    if(a[1]==='SELF')contextState.combat.self=side;
    if(a[1]==='TARGET')contextState.combat.target=side;
    renderContext();
  }
}
renderContext();

function xterm256(n){
  n=Number(n);
  if(n<16){
    const fallback=['#000','#800000','#008000','#808000','#000080','#800080','#008080','#c0c0c0','#808080','#ff0000','#00ff00','#ffff00','#0000ff','#ff00ff','#00ffff','#fff'];
    return fallback[n] || '';
  }
  if(n>=232){const v=8+(n-232)*10;return `rgb(${v},${v},${v})`;}
  n-=16;const r=Math.floor(n/36),g=Math.floor((n%36)/6),b=n%6;const cv=i=>i===0?0:55+i*40;
  return `rgb(${cv(r)},${cv(g)},${cv(b)})`;
}
function terminalSafe(s){
  return String(s)
    .replace(/\x1b\][^\x07]*(?:\x07|\x1b\\)/g,'')
    .replace(/\x1b[78]/g,'')
    .replace(/\x1b\[[0-?]*[ -\/]*[@-~]/g,m=>m.endsWith('m')?m:'');
}
// A "bar run" is a solid-colored chunk of nothing but ■/□/space -- the raw
// material the score/hp status bars are built from (tribar_graph() in
// cmds/usr/score.c). Rendered as ordinary text they inherit the terminal's
// own (proportional, non-square) font metrics, so they come out thin and
// undersized next to the reference client's blocky bars. Instead each unit
// becomes its own fixed-size square cell: ■ / a colored space becomes a
// solid-filled cell, □ becomes a hollow (border-only) cell so the
// filled/unfilled distinction the original glyphs conveyed still reads.
function isBarRun(text,fg,bg){ return (fg||bg) && /^[■□ ]+$/.test(text); }
function barCells(text,fg,bg){
  let html='';
  for(const ch of text){
    html+= ch==='□'
      ? `<span class="stat-cell stat-cell-empty" style="border-color:${fg||'currentColor'}"></span>`
      : `<span class="stat-cell" style="background:${bg||fg}"></span>`;
  }
  return html;
}
function ansi(s){
  s=terminalSafe(s);
  let html='',last=0,bold=false,underline=false,fg='',bg='';
  const re=/\x1b\[([0-9;]*)m/g;let m;
  const emit=(text,fg,bg,bold,underline)=>{
    if(!text)return;
    if(isBarRun(text,fg,bg)){ html+=barCells(text,fg,bg); return; }
    const st=(underline?'text-decoration:underline;':'')+(fg?`color:${fg};`:'')+(bg?`background:${bg};`:'');
    html+= st? `<span style="${st}">${esc(text)}</span>` : esc(text);
  };
  while((m=re.exec(s))){
    emit(s.slice(last,m.index),fg,bg,bold,underline);
    const cs=(m[1]||'0').split(';').filter(x=>x!=='').map(Number);if(!cs.length)cs.push(0);
    for(let i=0;i<cs.length;i++){
      const c=cs[i];
      if(c===0){bold=false;underline=false;fg='';bg='';}
      else if(c===1)bold=true;
      else if(c===4)underline=true;
      else if(c===22)bold=false;
      else if(c===24)underline=false;
      else if(c===39)fg='';
      else if(c===49)bg='';
      else if(baseColors[c])fg=baseColors[c];
      else if(bgColors[c])bg=bgColors[c];
      else if((c===38||c===48) && cs[i+1]===5 && Number.isFinite(cs[i+2])){const col=xterm256(cs[i+2]);if(c===38)fg=col;else bg=col;i+=2;}
    }
    last=re.lastIndex;
  }
  emit(s.slice(last),fg,bg,bold,underline);
  return html;
}
function cleanText(s){return terminalSafe(String(s)).replace(/\x1b\[[0-9;]*m/g,'').replace(/\r/g,'');}

const passwordPrompts=['請輸入密碼:','請設定您的密碼:','請重設您的密碼:','請再輸入一次您的密碼﹐以確認您沒記錯:','您兩次輸入的密碼並不一樣﹐請重新設定一次密碼:'];
// 留言／寫信的逐行編輯器：期間每一行輸入都會被收進內容，背景狀態查詢必須暫停，
// 直到玩家輸入「.」（完成）或「~q」（取消）。
let editorActive=false;
// 伺服器說「正在等玩家輸入」（選單、Yes/No、寫文章、巫師 edit…）：@@WEBHUD|INPUT 暫停背景狀態查詢，
// 回到指令提示時 @@WEBHUD|READY 恢復。查詢若在這時送出，會被當成玩家的回答或寫進檔案。
let serverInputWait=false;
function updateInputMode(chunk){
  if(cleanText(chunk).includes("結束離開用 '.'"))editorActive=true;
  tail=(tail+cleanText(chunk)).slice(-700);
  const secret=passwordPrompts.some(x=>tail.includes(x));
  const wasSecret=input.type==='password';
  input.type=secret?'password':'text';input.autocomplete=secret?'current-password':'off';
  // The "keep last command in the box" feature (see #form submit) leaves
  // whatever was just typed sitting in the field -- fine between ordinary
  // commands, but if the very next prompt turns out to be a password, that
  // leftover text (e.g. the username just entered) must not sit there
  // pre-filled/selected in the masked field.
  if(secret&&!wasSecret){input.value='';repeatArmed=false;}
  const mode=document.querySelector('#inputMode');if(mode){mode.textContent=secret?'密碼輸入':'指令輸入';mode.classList.toggle('secret',secret);}
}

function classifyLine(line){
  const plain=cleanText(line).trim();
  if(!plain)return '';
  // 巫師看得到的「房間名 - /路徑」格式直接視為房間標題，不依賴地圖資料是否已收錄。
  if(/^\S+ - \/[\w\/.-]+$/.test(plain))return 'room-line';
  // 人物那一行結尾是「(英文代號)」，例如「同仁藥鋪 掌櫃 (Keeper)」。頭銜剛好和房間同名時，
  // 不能因為開頭像房間名稱就被當成房間標題上色。
  if(isRoomLabel(plain)&&!/\s\([A-Za-z][^()]*\)$/.test(plain))return 'room-line';
  if(/^出口\s*[:：]/.test(plain)||/明顯的出口|唯一的出口/.test(plain))return 'exit-line';
  // 其他文字的顏色一律由遊戲（伺服器送來的 ANSI 顏色）決定，網頁不再依關鍵字替整行上色；
  // 例如攻擊敘述含「擊中」、人物名稱含「受傷」，都不該因字面被染成紅色。
  return '';
}
// 伺服器在每段輸出前後都會送提示字元「> 」，並在它前後各帶一個換行。網頁有輸入框，
// 提示字元不需要顯示；只拿掉「>」而留下那些換行，畫面就會多出一行行空白
// （潛行、戰鬥這類每個心跳印一行的訊息最明顯）。這裡逐行處理：
//   - \r\r\n、\r\n 都只算一次換行
//   - 整行只有提示字元：連同它前面帶出的空行一起拿掉；正在等輸入的提示字元（在一段的最後）
//     拿掉後，下一段開頭跟著它的那個換行也一併拿掉
//   - 接在文字前面的「> 」只拿掉提示字元（前面有色碼的「> 」是內容，例如留言板的引文，不動）
//   - 只有色碼、沒有文字的行接到下一行，不要佔成一個空行
let promptPending=false;
function isPromptOnly(line){return /^[ \t]*>[ \t]*$/.test(cleanText(line));}
function compactGameOutput(s){
  let text=String(s).replace(/\r+\n/g,'\n').replace(/\r/g,'');
  if(promptPending&&text.startsWith('\n'))text=text.slice(1);
  promptPending=false;
  const lines=text.split('\n');
  let partial=lines.pop();
  const kept=[];
  let carry='';   // 只有色碼的行，接到下一行前面
  for(const raw of lines){
    if(isPromptOnly(raw)){
      if(kept.length&&kept[kept.length-1]==='')kept.pop();
      continue;
    }
    let line=carry+raw.replace(/^[ \t]*>[ \t]+(?=\S)/,'');carry='';
    if(!cleanText(line)&&/\x1b\[/.test(line)){carry=line;continue;}
    kept.push(line);
  }
  if(isPromptOnly(partial)){
    if(kept.length&&kept[kept.length-1]==='')kept.pop();
    partial='';promptPending=true;
  }else partial=partial.replace(/^[ \t]*>[ \t]+(?=\S)/,'');
  partial=carry+partial;
  let out=kept.length?kept.join('\n')+'\n'+partial:partial;
  // Keep deliberate paragraph spacing, but prevent MUD output from turning repeated
  // empty lines into large blank vertical gaps in the browser terminal.
  out=out.replace(/\n(?:[ \t]*\n){2,}/g,'\n\n');  // 連續空行縮成一行，但保留下一行開頭的空白（置中排版用）
  return out;
}
function renderChunk(s){
  const parts=compactGameOutput(s).split(/(\r\n|\n|\r)/);let out='';
  for(let i=0;i<parts.length;i++){
    const p=parts[i];
    if(p==='\n'||p==='\r'||p==='\r\n'){out+='<br>';continue;}
    if(!p)continue;
    const cls=classifyLine(p);out+=cls?`<span class="${cls}">${ansi(p)}</span>`:ansi(p);
  }
  return out;
}
function isNearBottom(){return term.scrollHeight-term.scrollTop-term.clientHeight<90;}
// 是否貼底改由捲動事件維護，不在每段輸出時讀 scrollHeight（會強迫瀏覽器立即排版）；
// 貼底時的自動捲動也合併成每個畫面影格最多一次，分頁在背景時則完全不做。
let stickToBottom=true;
let autoScrollTop=-1;
let stickScrollFrame=0;
function shouldStick(){return stickToBottom;}
function scheduleStickScroll(){
  if(stickScrollFrame)return;
  stickScrollFrame=requestAnimationFrame(()=>{
    stickScrollFrame=0;
    if(!stickToBottom)return;
    term.scrollTop=term.scrollHeight;
    autoScrollTop=term.scrollTop;
    updateScrollLatest();
  });
}
function hpLineKind(line){
  const p=cleanText(line).trim();
  if(!p)return 'blank';
  if(/^>\s*hp\s*$/i.test(p)||/^hp\s*$/i.test(p))return 'command';
  if(/^>\s*$/.test(p))return 'prompt';
  let hits=0;
  for(const label of ['形體','精','氣','神','食物','飲水','疲勞']){
    if(new RegExp(label+'\\s*-?\\d+\\s*\\/\\s*-?\\d+').test(p))hits++;
  }
  return hits>=2?'summary':'other';
}
function isHpSummaryLine(line){
  return hpLineKind(line)==='summary';
}
// Strip @@WEBHUD|...| lines out of a buffer, keeping only real text.
// 標記也可能接在同一行的提示字元「> 」或其他文字後面，所以從標記處截斷，只保留前面的真正文字。
// 內部狀態資料的判斷：只有在「行首」（前面最多是提示字元「> 」）出現 @@WEBHUD| 的一整行才算。
// 文章、聊天內容中間提到 @@WEBHUD|BEGIN 這串字時是一般文字，照常顯示，也不會讓網頁誤以為資料開始而卡住。
const HUD_LINE_RE=/^[ \t]*(?:>[ \t]*)*@@WEBHUD\|/;
function isHudLine(line){ return HUD_LINE_RE.test(cleanText(line)); }
function hasHudLine(text,tag){
  return new RegExp('(?:^|\\n)[ \\t]*(?:>[ \\t]*)*@@WEBHUD\\|'+tag).test(cleanText(text));
}
// 只刪除內部狀態資料行，其他文字（包含空行）原樣保留。
function removeHudLines(text){
  const parts=String(text).split(/(\r\n|\n|\r)/);
  let out='';
  for(let i=0;i<parts.length;i+=2){
    const line=parts[i], sep=parts[i+1]||'';
    if(isHudLine(line))continue;
    out+=line+sep;
  }
  return out;
}
// 傳輸被切在一行資料中間時（例如只收到「@@WEBHUD」，「|BEGIN」在下一段），
// 最後那半行先留著等下一段，不要當成一般文字顯示。
function isHudLinePrefix(line){
  const plain=cleanText(line).replace(/^[ \t]*(?:>[ \t]*)*/,'');
  return !!plain && ('@@WEBHUD|'.startsWith(plain)||plain.startsWith('@@WEBHUD|'));
}
function holdTrailingHudPartial(text){
  const str=String(text);
  const nl=Math.max(str.lastIndexOf('\n'),str.lastIndexOf('\r'));
  const last=str.slice(nl+1);
  if(!isHudLinePrefix(last))return str;
  hudFallbackBuffer=last+hudFallbackBuffer;
  return str.slice(0,nl+1);
}
function stripWebHudLines(whole){
  return String(whole).split(/\r\n|\n|\r/).filter(line=>{
    if(isHudLine(line))return false;
    const plain=cleanText(line).trim();
    if(!plain)return false;
    if(/^>\s*$/.test(plain))return false;
    if(/^webhud$/i.test(plain))return false;
    return true;
  });
}
// 最後一道防線：畫面文字裡若還有行首的 @@WEBHUD| 資料行，先解析再刪掉，絕不顯示在終端機上。
function consumeHudPollOutput(s){
  const r=consumeHudPollOutputRaw(s);
  if(r.visible&&hasHudLine(r.visible,'')){
    parseWebHud(r.visible);
    r.visible=removeHudLines(r.visible);
  }
  return r;
}
function consumeHudPollOutputRaw(s){
  if(!hudPollInFlight){
    // 不在輪詢中：逾時後才到的半段回覆也可能出現在這裡。逐段處理、立刻顯示一般文字，
    // 只把行首的資料行刪掉，不再整段暫存等結尾（文章內文提到標記時會因此卡住）。
    // 唯一暫存的是「被拆在兩段傳輸之間、還沒收完的那一行資料」。
    let str=hudFallbackBuffer+String(s);
    hudFallbackBuffer='';
    // 遊戲每段回覆開頭都有一個換行；逾時才到的狀態回覆，開頭那個換行若照常顯示就成了空行。
    // 只有空白的片段先留著，等下一段到了再判斷是不是狀態回覆的開頭。
    if(!/\S/.test(str)){hudFallbackBuffer=str;return {visible:'',done:false};}
    if(!str.includes('@'))return {visible:str,done:false};
    str=holdTrailingHudPartial(str);
    if(hasHudLine(str,'')){
      parseWebHud(str);
      str=str.replace(/(?:^|[\r\n])[ \t\r\n]*(?=[ \t]*(?:>[ \t]*)*@@WEBHUD\|)/g,(m,off)=>off===0?'':'\n');
      // 整段只是狀態回覆（剩下的只有提示字元「>」和空白）時，什麼都不顯示。
      const rest=removeHudLines(str);
      return {visible:/[^\s>]/.test(cleanText(rest))?rest:'',done:false};
    }
    return {visible:removeHudLines(str),done:false};
  }
  hudPollBuffer += String(s);
  parseWebHud(hudPollBuffer);
  const clean=cleanText(hudPollBuffer);
  const hasEnd=hasHudLine(hudPollBuffer,'END(?:[ \\t]*)(?:\\n|$)');
  const timedOut=Date.now()-hudPollStartedAt>HUD_POLL_TIMEOUT_MS;
  if(!hasEnd && !timedOut)return {visible:'',done:false};
  const whole=holdTrailingHudPartial(hudPollBuffer);
  hudPollInFlight=false;hudBootstrapInFlight=false;hudPollBuffer='';hudPollStartedAt=0;
  if(hudResponseTimer){clearBg(hudResponseTimer);hudResponseTimer=null;}
  flushPendingUserCommands();
  const kept=stripWebHudLines(whole);
  // HUD polling is a silent transport. Internal payload/prompt whitespace must never
  // create a visible terminal line. Real asynchronous text is still preserved.
  return {visible:kept.length?kept.join('\n')+'\n':'',done:true};
}
function hudPollDue(){
  const now=Date.now();
  // 剛下指令：指令之後還沒查過就立刻查一次，之後同戰鬥中的節奏，不連續狂查。
  if(now-lastUserCommandAt<HUD_AFTER_COMMAND_MS){
    if(lastHudPollAt<lastUserCommandAt)return true;
    return now-lastHudPollAt>=HUD_POLL_MS;
  }
  const gap=contextState.combat?.target?HUD_POLL_MS:HUD_IDLE_POLL_MS;
  return now-lastHudPollAt>=gap;
}
function hudPollReady(force=false,allowNoRoom=false){
  if((!currentRoomId&&!allowNoRoom)||input.type==='password'||ws?.readyState!==1||hudPollInFlight)return false;
  if(serverInputWait)return false;
  if(document.hidden)return false;
  if(!force&&!hudPollDue())return false;
  if(editorActive)return false;
  // Real player input has priority over internal HUD telemetry.
  if(!force && Date.now()-lastUserCommandAt<HUD_USER_GRACE_MS)return false;
  // HUD must continue updating during combat even when no classic ">" prompt is emitted.
  // Only hold polling while the newest server text is an interactive login/choice prompt.
  const recent=tail.trim().split('\n').slice(-2).join(' ').slice(-220);
  // 魂飛魄散後重新選種族、性別、名稱時，舊的房間代號還在；不擋的話背景查詢會被當成答案，畫面一直跳「請重新輸入」。
  if(/請輸入(?:選項|.*數字)|請輸入\s*Y\s*或\s*N|密碼|是否.*[？?]|確定嗎|請重新輸入|您的選擇|種族名|\(Male \/ Female\)|\(Yes \/ No\)|顯示名稱|使用者代號|電子郵件/.test(recent))return false;
  return true;
}
function pollHud(force=false,allowNoRoom=false){
  if(!hudPollReady(force,allowNoRoom))return false;
  observer.begin('hp','webhud');
  hudPollInFlight=true;
  hudPollBuffer='';
  hudPollStartedAt=Date.now();
  lastHudPollAt=hudPollStartedAt;
  ws.send('webhud\r\n');
  if(allowNoRoom)hudBootstrapInFlight=true;
  if(hudResponseTimer)clearBg(hudResponseTimer);
  // 背景計時：網頁不在最上層時也要準時放行排隊中的指令（觸發、計時器送出的指令也在這裡排隊）。
  hudResponseTimer=bgTimeout(()=>{
    hudResponseTimer=null;
    if(!hudPollInFlight)return;
    // Never let a missing/split HUD END marker hold real player clicks forever.
    hudBootstrapInFlight=false;
    releaseHudPoll();
    flushPendingUserCommands();
    if(!currentRoomId&&!exactRoomTrackingReady)scheduleMapBootstrapRetry(180);
  },HUD_POLL_TIMEOUT_MS);
  return true;
}
// 放棄等待一個逾時（或被玩家指令搶先）的 webhud 回覆。等待期間收到的文字可能混著
// 真正的遊戲輸出（例如 read 1 的文章內容、別人說話），以前直接連同緩衝一起丟掉，
// 畫面上就只剩「> read 1」而沒有任何回應。現在只拿掉 @@WEBHUD 資料，其餘照常顯示；
// 之後才到的半段 HUD 回覆由 consumeHudPollOutput 的最後防線過濾。
function releaseHudPoll(){
  const whole=holdTrailingHudPartial(hudPollBuffer);
  hudPollInFlight=false;hudPollBuffer='';hudPollStartedAt=0;
  if(hudResponseTimer){clearBg(hudResponseTimer);hudResponseTimer=null;}
  if(!whole)return;
  parseWebHud(whole);
  const kept=stripWebHudLines(whole);
  if(kept.length)renderVisible(kept.join('\n')+'\n',false,shouldStick());
}
// 切回分頁時立刻更新一次狀態。
document.addEventListener('visibilitychange',()=>{ if(!document.hidden)pollHud(true); });
function startHudPolling(){
  if(hudTimer)return;
  hudTimer=setInterval(pollHud,HUD_POLL_MS);
  setTimeout(pollHud,120);
}
function stopHudPolling(){
  if(hudTimer){clearInterval(hudTimer);hudTimer=null;}
  if(hudKickTimer){clearTimeout(hudKickTimer);hudKickTimer=null;}
  if(hudResponseTimer){clearBg(hudResponseTimer);hudResponseTimer=null;}
  hudPollInFlight=false;hudPollBuffer='';hudPollStartedAt=0;
  pendingUserCommands=[];
}
function kickHudAfterServerText(force=false){
  if(hudKickTimer)clearTimeout(hudKickTimer);
  hudKickTimer=setTimeout(()=>{hudKickTimer=null;pollHud(force);},HUD_KICK_DELAY_MS);
}
function print(s,system=false){
  const stick=shouldStick();
  lastReceiveAt=Date.now();
  if(!system){
    // 背景 webhud 輪詢（每 0.8 秒）的回應不是玩家指令的輸出，不寫進 session 紀錄，
    // 否則掛機時上一個指令的回應會被 HUD 資料無限撐大。
    if(!hudPollInFlight&&!String(s).includes('@@WEBHUD|'))sessionRecorder.appendResponse(s);
    captureChatMessages(s);
  }
  // 萬一沒收到恢復記號：等待輸入中又看到一般的指令提示「>」結尾，就當作已回到指令狀態。
  if(serverInputWait&&!system&&!String(s).includes('@@WEBHUD|INPUT')&&/(?:^|\n)[ \t]*>[ \t]*$/.test(cleanText(String(s))))serverInputWait=false;
  observer.consume(s);renderObserved();updateInputMode(s);observeRoomText(s);bootstrapExactRoomFromOutput(s);

  let shown=String(s);
  let prefix='';
  if(!system){
    // 橋接程式連上遊戲時送出的「[ES2 connected]」只給工具判斷用，畫面上不顯示。
    shown=shown.replace(/^[\r\n]*\[ES2 connected\][ \t]*\r?\n?/,'');
    shown=consumeHudPollOutput(shown).visible;
  }
  if(prefix)appendTerminalHtml(prefix,true);
  renderVisible(shown,system,stick);
}
function renderVisible(shown,system,stick){
  if(!system&&shown)automation.feed(shown,cleanText);
  let rendered=shown?renderChunk(shown):'';
  if(!system&&pendingTrimLeadingBlank){
    // Server output can arrive split across several WebSocket frames -- e.g.
    // a stray driver prompt fragment renders (after compactGameOutput strips
    // the literal "&gt;") as pure "<br>"s with no real text of its own, in a
    // frame all by itself before the actual response text. Strip leading
    // line breaks from the RENDERED html (so this works regardless of which
    // frame the server happened to split the leading blank line into), and
    // suppress a frame that turns out to be nothing but those breaks rather
    // than inserting a visible gap, staying armed for the next frame.
    const strippedRendered=rendered.replace(/^(?:<br>)+/,'');
    if(strippedRendered.trim()){rendered=strippedRendered;pendingTrimLeadingBlank=false;}
    else rendered='';
  }
  if(rendered)appendTerminalHtml(`<span class="${system?'sys':''}">${rendered}</span>`,rendered.endsWith('<br>'));
  trimTerminal(stick);
  if(stick)scheduleStickScroll();
}
// 訊息欄內容以「段落區塊」為單位附加：上一段以換行結尾時，新輸出放進新的
// <div>；否則接在同一個區塊裡（同一行的延續）。視覺上與全部塞在同一層 inline
// 完全相同，但原本整個訊息欄是一個巨大的 inline 段落，每來一段文字（並讀取
// scrollHeight 判斷是否貼底）瀏覽器就得把全部歷史重新排版；分成區塊後只需排版新增的部分。
let termBlock=null;
let termBlockOpen=false;
function appendTerminalHtml(html,endsLine){
  if(!html)return;
  if(!termBlockOpen||!termBlock||termBlock.parentNode!==term){
    termBlock=document.createElement('div');
    termBlock.className='term-block';
    term.appendChild(termBlock);
  }
  termBlock.insertAdjacentHTML('beforeend',html);
  termBlockOpen=!endsLine;
}
// 訊息欄只保留最近的內容。原本每段輸出都永久留在 DOM，掛 3~4 小時（尤其戰鬥中）
// 會累積數十萬個節點，每次新訊息都要對整棵樹排版，瀏覽器越跑越吃 CPU/記憶體。
// 超過上限時一次刪掉一批最舊的，攤平成本；玩家正在往上捲閱讀時保持畫面位置不跳動。
const TERMINAL_MAX_NODES=3000;
const TERMINAL_TRIM_TO=2400;
function trimTerminal(stick){
  const extra=term.childNodes.length-TERMINAL_MAX_NODES;
  if(extra<=0)return;
  const removeCount=term.childNodes.length-TERMINAL_TRIM_TO;
  const before=stick?0:term.scrollHeight;
  const range=document.createRange();
  range.setStartBefore(term.firstChild);
  range.setEndAfter(term.childNodes[removeCount-1]);
  range.deleteContents();
  // 刪掉上方內容後瀏覽器會自行調整 scrollTop 並送出捲動事件；記下調整後的位置，
  // 以免被當成玩家往上捲而取消貼底。
  if(stick)autoScrollTop=term.scrollTop;
  else term.scrollTop=Math.max(0,term.scrollTop-(before-term.scrollHeight));
}

function parsePairFromState(key){
  // Silent webhud is the live source. Manual score output is only a fallback, never
  // allowed to override newer background vitals with stale values.
  const live=observer.state.vitals?.[key];if(live)return {current:live.current,max:live.limit,eff:live.effective??live.limit};
  const detail=observer.state.scoreDetail?.stats?.[key];if(detail)return {current:detail.current,max:detail.maximum,eff:detail.effective??detail.maximum};
  return null;
}
function renderScoreHUD(){ renderObserved(); }

let lastScoreHudHtml=null;
function renderObserved(){
  // 版面跟 score 一致：形體／精／氣／神是「名稱 數字 方格」一行；食物／飲水／疲勞只顯示文字，排在最下面一行。
  const labels={hp:'形體',jing:'精',qi:'氣',shen:'神',food:'食物',water:'飲水',fatigue:'疲勞'};
  const colors={hp:'#2f7a4a',jing:'#efd06b',qi:'#ff7474',shen:'#76a9ff'};
  const CELLS=15;
  const num=v=>`${String(v.current).padStart(3)}/${String(v.max).padStart(4)}`;
  const vitals=[],needs=[];
  for(const key of ['hp','jing','qi','shen']){
    const v=parsePairFromState(key);if(!v)continue;
    const pct=v.max>0?Math.max(0,Math.min(100,Math.round(v.current*100/v.max))):0;
    // 跟 score 的 tribar 一樣：目前值是實心格，目前值到有效上限是空心格（形體為暗紅），有效上限以上不畫。
    const clamp=n=>Math.max(0,Math.min(CELLS,n));
    const filled=v.max>0?clamp(Math.floor(v.current*CELLS/v.max)):0;
    const effCells=v.max>0?clamp(Math.floor((v.eff??v.max)*CELLS/v.max)):0;
    const cells=Array.from({length:CELLS},(_,i)=>`<i class="hud-cell${i<filled?' filled':i<effCells?'':' gone'}" aria-hidden="true"></i>`).join('');
    vitals.push(`<div class="hud-vital hud-${key}" style="--hud-color:${colors[key]}"><span class="hud-label">${labels[key]}</span><b class="hud-num">${num(v)}</b><div class="hud-cells" role="img" aria-label="${labels[key]} ${pct}%">${cells}</div></div>`);
  }
  // 最下面一行跟上面共用欄位：食物的數字靠右對齊精氣神的最大值，疲勞的數字靠右對齊方格的最右邊，飲水在兩者中間。
  const need=key=>{const v=parsePairFromState(key);return v?`<span class="hud-need hud-${key}"><span class="hud-label">${labels[key]}</span><b class="hud-num">${num(v)}</b></span>`:'';};
  const food=parsePairFromState('food');
  const rest=need('water')+need('fatigue');
  if(food||rest)needs.push(`<div class="hud-vital hud-needs"><span class="hud-label">${food?labels.food:''}</span><b class="hud-num">${food?num(food):''}</b><div class="hud-needs-rest">${rest}</div></div>`);
  const hud=document.querySelector('#scoreHud');
  if(hud&&(vitals.length||needs.length)){
    const html=`<div class="hud-vitals">${vitals.join('')}${needs.join('')}</div>`;
    if(html!==lastScoreHudHtml){hud.innerHTML=html;lastScoreHudHtml=html;}
    hud.classList.remove('muted');
  }
  const level=observer.state.scoreDetail?.level ?? observer.state.score?.level;
  const lev=document.querySelector('#levelHud');if(lev)lev.textContent=level!==undefined?`Lv.${level}`:'Lv.--';
}

const vec={north:[0,-1],south:[0,1],east:[1,0],west:[-1,0],northeast:[1,-1],northwest:[-1,-1],southeast:[1,1],southwest:[-1,1]};
const verticalDirs=new Set(['up','down']);
const exploredRooms=new Set();
const roomMapMeta=new Map();
let runtimeSnapshot=null;
let exactRoomTrackingReady=false;
const MAP_CACHE_KEY='es2.mapExplored.v7.readonlyworld';
const STATIC_WORLD_MAP=true;
let predictedRoomId=null;
let mapCacheSaveTimer=null;
// V7: localStorage may remember exploration only. The shipped Map DB is immutable.
function loadTopologyCache(){
  try{
    const raw=localStorage.getItem(MAP_CACHE_KEY);if(!raw)return;
    const cache=JSON.parse(raw);if(!cache||typeof cache!=='object')return;
    for(const id of cache.explored||[])if(nodeById.has(id))exploredRooms.add(id);
  }catch(_){/* ignore corrupt browser cache */}
}
function scheduleTopologyCacheSave(){
  clearTimeout(mapCacheSaveTimer);mapCacheSaveTimer=setTimeout(()=>{
    try{localStorage.setItem(MAP_CACHE_KEY,JSON.stringify({explored:[...exploredRooms]}));}
    catch(_){/* storage full/private mode */}
  },120);
}

function normalizeRoomLabel(s){return String(s).replace(/^【|】$/g,'').replace(/^\[|\]$/g,'').trim();}
function indexGraph(){
  nodeById=new Map(graph.nodes.map(n=>[n.id,n]));labelIndex=new Map();adjacency=new Map(graph.nodes.map(n=>[n.id,[]]));
  for(const n of graph.nodes){const key=normalizeRoomLabel(n.label);if(!labelIndex.has(key))labelIndex.set(key,[]);labelIndex.get(key).push(n.id);}
  for(const e of graph.edges){if(!e.resolved||!nodeById.has(e.from)||!nodeById.has(e.to))continue;adjacency.get(e.from)?.push(e);}
}
async function loadGraph(){
  try{
    // V7: the shipped DB owns room IDs, exits and coordinates. Runtime is read-only.
    const r=await fetch('/world_static_map.json',{cache:'no-store'});if(!r.ok)throw Error(String(r.status));graph=await r.json();
    indexGraph();loadTopologyCache();loadFixedWorldPositions();
    const layerHud=document.querySelector('#mapLayerHud');if(layerHud)layerHud.textContent='MAP V8 唯讀全圖｜自動定位';
    renderLocalMap();}
  catch(e){document.querySelector('#localMap').innerHTML='<div class="map-empty">固定世界地圖尚未建立</div>';}
}
function isRoomLabel(line){
  const text=normalizeRoomLabel(String(line).replace(/^\s+|\s+$/g,''));
  if(labelIndex.has(text))return true;
  for(const label of labelIndex.keys())if(text===label||text.startsWith(label+' '))return true;
  return false;
}
function roomCandidatesFromLine(line){
  const text=normalizeRoomLabel(cleanText(line).trim());const exact=labelIndex.get(text);if(exact)return exact;
  for(const [label,ids] of labelIndex){if(text===label||text.startsWith(label+' ')||text.startsWith('【'+label+'】'))return ids;}
  return [];
}
function chooseRoom(candidates){
  if(!candidates.length)return null;if(candidates.length===1)return candidates[0];
  if(currentRoomId){const near=new Set((adjacency.get(currentRoomId)||[]).map(e=>e.to));const hit=candidates.find(id=>near.has(id));if(hit)return hit;}
  return null;
}
function ensureRuntimeRoom(id,label){
  id=String(id||'').replace(/#\d+$/,'');if(!id)return null;
  // V7: runtime must never create, rename or otherwise mutate map rooms.
  return nodeById.has(id)?id:null;
}


function setRoomMapMeta(id,area,layer,mode){
  id=ensureRuntimeRoom(id,'');if(!id)return;
  roomMapMeta.set(id,{area:normalizeRoomLabel(area)||area||'',layer:normalizeRoomLabel(layer)||layer||'地面',mode:String(mode||'').toLowerCase()});
  scheduleTopologyCacheSave();
  if(id===currentRoomId&&!runtimeSnapshot)renderLocalMap();
}
function mapMeta(id){
  // The packaged Map DB now carries each room's area/layer baked in at build
  // time, so this is available the instant a room id is known -- including
  // during the optimistic (pre-server-confirmation) move -- with no need to
  // wait for the live WEBHUD round-trip. Live MAPMETA data, once it arrives,
  // still takes precedence (kept as the authoritative/most current source).
  const live=roomMapMeta.get(id);
  const node=nodeById.get(id);
  return {
    area:(live&&live.area)||(node&&node.area)||'',
    layer:(live&&live.layer)||(node&&node.layer)||'地面',
    mode:(live&&live.mode)||''
  };
}
function sameMapLayer(a,b){const ma=mapMeta(a),mb=mapMeta(b);return !ma.layer||!mb.layer||ma.layer===mb.layer;}
function sameMapArea(a,b){const ma=mapMeta(a);if(!ma.area)return true;const mb=mapMeta(b);return !mb.area?false:ma.area===mb.area;}

function beginRuntimeRoomSnapshot(id,label){
  const rawId=String(id||'').replace(/#\d+$/,'');
  id=ensureRuntimeRoom(rawId,label);
  if(!id){
    // A room path not present in the packaged DB must never permanently disable
    // bootstrap. Keep retrying telemetry, but do not mutate the read-only map.
    lastMissingMapRoomId=rawId;
    exactRoomTrackingReady=false;currentRoomId=null;predictedRoomId=null;hudBootstrapInFlight=false;
    renderLocalMap();scheduleMapBootstrapRetry();return;
  }
  // First exact ROOM path permanently disables room-name guessing. Duplicate room
  // names can never move the marker again.
  exactRoomTrackingReady=true;lastMissingMapRoomId='';
  runtimeSnapshot={id,old:''};
  const changed=id!==currentRoomId;
  currentRoomId=id;predictedRoomId=null;exploredRooms.add(id);
  hudBootstrapInFlight=false;hudBootstrapAttempts=0;
  if(changed)renderLocalMap();
  startHudPolling();
}
function finalizeRuntimeRoomSnapshot(){
  if(!runtimeSnapshot)return;runtimeSnapshot=null;
  scheduleTopologyCacheSave();
  // V5 deliberately does not rebuild or recalculate the map here. HUD completion
  // is telemetry only. The visible map has already moved synchronously on input.
}

function mergeRuntimeExit(from,direction,to,toLabel){
  // V7: WEBHUD exits are telemetry/validation only. The packaged Map DB is authoritative.
  from=String(from||'').replace(/#\d+$/,'');to=String(to||'').replace(/#\d+$/,'');
  direction=String(direction||'').toLowerCase();
  if(!nodeById.has(from)||!nodeById.has(to)||!direction)return false;
  return (adjacency.get(from)||[]).some(e=>e.direction===direction&&e.to===to);
}

function setExactRoom(id,label){
  const rawId=String(id||'').replace(/#\d+$/,'');
  id=ensureRuntimeRoom(rawId,label);if(!id){
    lastMissingMapRoomId=rawId;exactRoomTrackingReady=false;currentRoomId=null;renderLocalMap();scheduleMapBootstrapRetry();return;
  }
  exactRoomTrackingReady=true;lastMissingMapRoomId='';
  if(id!==currentRoomId){currentRoomId=id;exploredRooms.add(id);scheduleTopologyCacheSave();renderLocalMap();}
  const crt=document.querySelector('#consoleRoomTitle');if(crt)crt.textContent=safeMapLabel(label||nodeById.get(id)?.label||'')||'東方故事 II';
}
function looksLikeWorldRoomOutput(chunk){
  const text=cleanText(String(chunk||''));
  if(!text.trim())return false;
  // Examine a rolling receive window, not one WebSocket packet. Neolith may split
  // the room title and exit sentence across separate TCP/WebSocket chunks.
  if(/明顯的出口|唯一的出口|沒有明顯的出口|出口\s*[:：]/.test(text))return true;
  for(const raw of text.split(/\r?\n/)){
    const line=raw.trim();
    if(/^.{1,32}\s+-\s*(?:\/[^\s]+)?\s*$/.test(line) && !/ES2|WebMUD|HTTP/i.test(line))return true;
  }
  return false;
}
function scheduleMapBootstrapRetry(delay=180){
  if(currentRoomId||exactRoomTrackingReady||input.type==='password'||ws?.readyState!==1)return;
  if(mapBootstrapRetryTimer)return;
  mapBootstrapRetryTimer=setTimeout(()=>{
    mapBootstrapRetryTimer=null;
    if(currentRoomId||exactRoomTrackingReady||input.type==='password'||ws?.readyState!==1)return;
    if(!looksLikeWorldRoomOutput(tail))return;
    if(hudBootstrapAttempts>=8)hudBootstrapAttempts=0;
    hudBootstrapAttempts++;
    if(!pollHud(true,true))scheduleMapBootstrapRetry(220);
  },delay);
}
function bootstrapExactRoomFromOutput(chunk){
  if(currentRoomId||exactRoomTrackingReady||hudBootstrapInFlight)return;
  if(input.type==='password'||ws?.readyState!==1)return;
  // `tail` is updated immediately before this call. Using it makes bootstrap
  // immune to room output being fragmented across network packets.
  if(!looksLikeWorldRoomOutput(tail))return;
  scheduleMapBootstrapRetry(40);
}
function observeRoomText(chunk){
  if(!graph.nodes.length||exactRoomTrackingReady)return;
  // Login/bootstrap fallback only. Once WEBHUD has supplied an exact LPC room path,
  // ordinary room prose must never move the map marker. Duplicate names such as
  // 村間/小路 previously caused a false jump, then WEBHUD corrected it 1-2s later.
  for(const line of cleanText(chunk).split('\n')){
    const id=chooseRoom(roomCandidatesFromLine(line));
    if(id&&id!==currentRoomId){currentRoomId=id;renderLocalMap();startHudPolling();break;}
  }
}
function safeMapLabel(raw){
  const s=normalizeRoomLabel(String(raw||'')).trim();
  if(!s||s.includes('/')||/^\.?\.?\//.test(s)||/^[/\\]/.test(s)||/^[A-Za-z0-9_.:-]+$/.test(s))return '';
  return s;
}
// 地圖格子裡顯示完整房名：兩個字一行，最多四行（八個字）。
function mapLabelLines(raw){
  const chars=[...safeMapLabel(raw).replace(/[\s　·・()（）【】\[\]]/g,'')].slice(0,8);
  const lines=[];for(let i=0;i<chars.length;i+=2)lines.push(chars.slice(i,i+2).join(''));
  return lines;
}
function twoCharMapLabel(raw){
  let s=safeMapLabel(raw);if(!s)return '';
  const rules=[
    [/李記當舖|李記當鋪|李記商行/,'當鋪'],[/小旅店|小飯店|客棧|客栈/,'旅店'],[/雜貨舖|雜貨鋪|雜貨店/,'雜貨'],
    [/藥舖|藥鋪|药铺|藥店|药店/,'藥鋪'],[/村口大門|村口大门/,'村門'],[/廣場中央|广场中央/,'中央'],
    [/溪邊小路|溪边小路|河邊|河边/,'溪邊'],[/西瓜田/,'瓜田'],[/石子路|石磚路|石砖路/,'石路'],[/街道/,'街道'],
    [/農家|农家/,'農家'],[/私塾/,'私塾'],[/福祠/,'福祠'],[/獵戶|猎户/,'獵戶'],[/小溪|溪流/,'小溪'],[/廣場|广场/,'廣場'],
    [/錢莊|钱庄/,'錢莊'],[/武館|武馆/,'武館'],[/鐵舖|铁铺/,'鐵舖'],[/酒樓|酒楼/,'酒樓'],[/茶館|茶馆/,'茶館'],
    [/軍營|军营/,'軍營'],[/草棚/,'草棚'],[/七彩石/,'彩石'],[/老松林|松林/,'松林'],[/家園大廳|家园大厅|大廳|大厅/,'大廳'],
    [/寢室|寝室/,'寢室'],[/書房|书房/,'書房'],[/演武場|演武场/,'演武'],[/城門|城门/,'城門'],[/堡門|堡门/,'堡門'],
    [/雪亭堡/,'雪堡'],[/虎刀門|虎刀门/,'虎門'],[/寺|廟|庙/,'寺廟'],[/宮|宫/,'宮殿'],[/府/,'府邸'],[/莊|庄/,'山莊']
  ];
  for(const [re,label] of rules)if(re.test(s))return label;
  const chars=[...s.replace(/[\s　·・()（）【】\[\]]/g,'')];
  return chars.slice(0,2).join('');
}
function mapNodeClass(raw){return safeMapLabel(raw)?'map-node-special':'map-node-known';}
let mapZoom=1;
function setMapZoom(next){
  mapZoom=Math.max(.72,Math.min(1.45,next));
  renderLocalMap();
  const lab=document.querySelector('#mapZoomLabel');if(lab)lab.textContent=`${Math.round(mapZoom*100)}%`;
}
const stableWorldPos=new Map();
function loadFixedWorldPositions(){
  stableWorldPos.clear();
  for(const n of graph.nodes||[]){
    const x=Number(n?.x),y=Number(n?.y);
    if(n?.id&&Number.isFinite(x)&&Number.isFinite(y))stableWorldPos.set(n.id,[x,y]);
  }
  if(stableWorldPos.size!==nodeById.size)throw new Error('Map DB coordinates incomplete');
}

function renderLocalMap(){
  const el=document.querySelector('#localMap');if(!el)return;
  const layerHud=document.querySelector('#mapLayerHud'),trans=document.querySelector('#mapTransitions');
  if(!currentRoomId||!nodeById.has(currentRoomId)){
    const msg=lastMissingMapRoomId?`固定地圖未收錄此房：${esc(lastMissingMapRoomId)}`:'正在定位所在房間…';
    el.innerHTML=`<div class="map-empty">${msg}</div>`;if(trans)trans.hidden=true;
    const areaTitle=document.querySelector('#mapAreaTitle');if(areaTitle)areaTitle.textContent='';
    return;
  }
  exploredRooms.add(currentRoomId);
  // 房間格子的大小固定（格子中心相距 62 像素）；地圖視窗拉大就看到更多格，不會把整張圖放大。
  // 縮放按鈕縮小時同樣多看幾格。
  const NS='http://www.w3.org/2000/svg',W=Math.max(120,el.clientWidth||360),H=Math.max(100,el.clientHeight||314),cx=W/2,cy=H/2,sx=62,sy=62;
  const radiusX=Math.ceil(W/2/mapZoom/sx),radiusY=Math.ceil(H/2/mapZoom/sy);
  const meta=mapMeta(currentRoomId),center=stableWorldPos.get(currentRoomId)||[0,0];
  if(layerHud)layerHud.textContent=`MAP V8 唯讀全圖 ｜ ${meta.area?meta.area+' ｜ ':''}${meta.layer||'地面'}`;
  const areaTitle=document.querySelector('#mapAreaTitle');
  if(areaTitle)areaTitle.textContent=meta.area?` - ${meta.area}`:'';
  const visible=new Map();
  for(const [id,p] of stableWorldPos){
    if(!nodeById.has(id)||!sameMapLayer(currentRoomId,id)||!sameMapArea(currentRoomId,id))continue;
    const dx=p[0]-center[0],dy=p[1]-center[1];if(Math.abs(dx)<=radiusX&&Math.abs(dy)<=radiusY)visible.set(id,[dx,dy]);
  }
  // Cross-area exits (e.g. the snow-town gate that leads into the Zhenwu camp)
  // don't carry a meaningful shared x/y with the current area -- each area is
  // laid out independently -- so the neighbour room is placed one grid step in
  // the exit's own direction instead of at its stored coordinates, and only
  // that single adjacent room is pulled in, not the rest of the other area.
  const boundarySet=new Set();
  for(const [id,p] of [...visible]){
    for(const e of adjacency.get(id)||[]){
      if(!vec[e.direction]||!sameMapLayer(currentRoomId,e.to)||sameMapArea(currentRoomId,e.to))continue;
      if(!visible.has(e.to)){const v=vec[e.direction];visible.set(e.to,[p[0]+v[0],p[1]+v[1]]);}
      boundarySet.add(id);boundarySet.add(e.to);
    }
  }
  const svg=document.createElementNS(NS,'svg');svg.setAttribute('viewBox',`0 0 ${W} ${H}`);svg.setAttribute('class','topology-svg');svg.style.transform=`scale(${mapZoom})`;
  const xy=(id)=>{const p=visible.get(id);return p?[cx+p[0]*sx,cy+p[1]*sy]:null;};
  const edgeSeen=new Set();
  for(const [from] of visible)for(const e of adjacency.get(from)||[]){
    if(!visible.has(e.to)||!vec[e.direction]||!sameMapLayer(currentRoomId,e.to))continue;const a=xy(from),b=xy(e.to);if(!a||!b)continue;
    const key=[from,e.to].sort().join('↔');if(edgeSeen.has(key))continue;edgeSeen.add(key);const line=document.createElementNS(NS,'line');
    line.setAttribute('x1',a[0]);line.setAttribute('y1',a[1]);line.setAttribute('x2',b[0]);line.setAttribute('y2',b[1]);line.setAttribute('class','map-edge'+((a[0]!==b[0]&&a[1]!==b[1])?' diagonal':''));svg.appendChild(line);
  }
  for(const [id] of visible){
    const [x,y]=xy(id),raw=nodeById.get(id)?.label||'',lines=mapLabelLines(raw),g=document.createElementNS(NS,'g');
    const cls=id===currentRoomId?'map-node-current':(boundarySet.has(id)?'map-node-boundary':mapNodeClass(raw));g.setAttribute('class',`map-node ${cls}`);
    // 房間格是 1:1 的正方形，房名完整顯示：兩個字一行、上下左右置中；
    // 一、二行（四個字以內）用一般字級，三、四行（五到八個字）字縮小一點，一樣放進同大小的格子。
    const tile=44,long=lines.length>2,lineH=long?10:14;
    const r=document.createElementNS(NS,'rect');r.setAttribute('x',x-tile/2);r.setAttribute('y',y-tile/2);r.setAttribute('width',tile);r.setAttribute('height',tile);r.setAttribute('rx',4);r.setAttribute('class','map-node-rect');g.appendChild(r);
    if(lines.length){
      const t=document.createElementNS(NS,'text');t.setAttribute('text-anchor','middle');t.setAttribute('class','map-node-label map-node-label-inside'+(long?' map-node-label-long':''));
      lines.forEach((line,i)=>{const ts=document.createElementNS(NS,'tspan');ts.setAttribute('x',x);ts.setAttribute('y',y+(i-(lines.length-1)/2)*lineH);ts.textContent=line;t.appendChild(ts);});
      g.appendChild(t);
    }
    if(id===currentRoomId){const dot=document.createElementNS(NS,'circle');dot.setAttribute('cx',x);dot.setAttribute('cy',y-tile/2);dot.setAttribute('r',2.6);dot.setAttribute('class','map-current-dot');g.appendChild(dot);}svg.appendChild(g);
  }
  el.replaceChildren(svg);
  const vertical=(adjacency.get(currentRoomId)||[]).filter(e=>verticalDirs.has(e.direction)||!vec[e.direction]||!sameMapLayer(currentRoomId,e.to));
  if(trans){trans.replaceChildren();for(const e of vertical){const box=document.createElement('span');box.className='map-transition';const mark=e.direction==='up'?'▲':e.direction==='down'?'▼':'◇';const target=twoCharMapLabel(nodeById.get(e.to)?.label||'');box.innerHTML=`<b>${mark}</b> ${esc(e.direction)}${target?'：'+esc(target):''}`;trans.appendChild(box);}trans.hidden=!vertical.length;}
}



const directionAliases={n:'north',s:'south',e:'east',w:'west',ne:'northeast',nw:'northwest',se:'southeast',sw:'southwest',u:'up',d:'down'};
function movementDirection(command){
  const parts=String(command||'').trim().toLowerCase().split(/\s+/).filter(Boolean);if(!parts.length)return '';
  let dir=parts[0]==='go'?(parts[1]||''):parts[0];return directionAliases[dir]||dir;
}
function optimisticMapMove(command){
  if(!currentRoomId)return false;
  const dir=movementDirection(command);if(!dir)return false;
  const edge=(adjacency.get(currentRoomId)||[]).find(e=>String(e.direction||'').toLowerCase()===dir);
  if(!edge?.to||!nodeById.has(edge.to))return false;
  predictedRoomId=edge.to;currentRoomId=edge.to;exploredRooms.add(edge.to);scheduleTopologyCacheSave();renderLocalMap();
  const crt=document.querySelector('#consoleRoomTitle'),label=safeMapLabel(nodeById.get(edge.to)?.label||'');if(crt&&label)crt.textContent=label;
  return true;
}



// #dis 主動斷線時不自動重連，等玩家 #con。
let manualDisconnect=false,reconnectTimer=null;
function connect(){
  if(ws&&[WebSocket.OPEN,WebSocket.CONNECTING].includes(ws.readyState))return;
  manualDisconnect=false;if(reconnectTimer){clearBg(reconnectTimer);reconnectTimer=null;}
  conn.textContent='CONNECTING';const proto=location.protocol==='https:'?'wss':'ws';ws=new WebSocket(`${proto}://${location.host}/mud`);ws.binaryType='arraybuffer';
  ws.onopen=()=>{
    // A TCP/WebSocket reconnect starts at the Neolith login object, not inside
    // the previous room. Never let stale room/HUD state emit internal `webhud`
    // commands into the username/password prompts.
    stopHudPolling();
    if(mapBootstrapRetryTimer){clearTimeout(mapBootstrapRetryTimer);mapBootstrapRetryTimer=null;}
    currentRoomId=null;
    runtimeSnapshot=null;
    exactRoomTrackingReady=false;
    tail='';
    hudPollInFlight=false;
    hudPollBuffer='';
    hudPollStartedAt=0;
    hudFallbackBuffer='';
    pendingUserCommands=[];
    mapRefreshPending=false;hudBootstrapInFlight=false;hudBootstrapAttempts=0;
    clearBg(retryResetTimer);retryResetTimer=bgTimeout(()=>{retryResetTimer=null;retry=0;},RECONNECT_STABLE_MS);
    conn.textContent='LIVE';sessionRecorder.markTransportOpen();
  };
  ws.onmessage=e=>{
    const raw=e.data instanceof ArrayBuffer?new TextDecoder('utf-8').decode(e.data):e.data;
    observeContext(raw);print(raw);
    // A completed movement command must refresh ROOM/NAME/EXITS immediately. The
    // normal 350ms user-input grace remains for all other commands so telemetry
    // cannot compete with player input.
    if(currentRoomId && /\S/.test(String(raw)) && !String(raw).includes('@@WEBHUD|')){
      const forceMapRefresh=mapRefreshPending;
      if(forceMapRefresh)mapRefreshPending=false;
      kickHudAfterServerText(forceMapRefresh);
    }
  };
  ws.onclose=e=>{stopHudPolling();editorActive=false;serverInputWait=false;if(mapBootstrapRetryTimer){clearTimeout(mapBootstrapRetryTimer);mapBootstrapRetryTimer=null;}pendingUserCommands=[];mapRefreshPending=false;hudBootstrapInFlight=false;hudBootstrapAttempts=0;currentRoomId=null;runtimeSnapshot=null;tail='';sessionRecorder.finishResponse();sessionRecorder.markTransportClose(e?.code??null,e?.reason||'');sessionRecorder.markReconnect();conn.textContent='DISCONNECTED';if(manualDisconnect){print('\n[已中斷連線，輸入 #con 重新連線]\n',true);return;}automation.cancelPending();if(!started)return;clearBg(retryResetTimer);retryResetTimer=null;const wait=reconnectDelay();print(`\n[${Math.round(wait/1000)} 秒後重新連線]\n`,true);reconnectTimer=bgTimeout(()=>{reconnectTimer=null;connect();},wait);};
  ws.onerror=()=>conn.textContent='ERROR';
}
// Echo the player's own command into the transcript as its own line, the
// way a real terminal locally echoes what you typed -- otherwise the log
// jumps straight from one room's text to the next with no record of what
// command caused the move. Skipped for password entry so a plaintext
// password never lands in the visible scrollback.
//
// Many server responses are themselves written with a leading blank line
// (e.g. "\n你再也喝不下東西了。\n"), meant to separate them from whatever
// came before when there was no echoed command line acting as that
// separator. Now that the echo line already marks the boundary, that
// leading blank line just leaves a gap between "> command" and its own
// response, so the next non-system print() after an echo has it trimmed.
let pendingTrimLeadingBlank=false;
function echoCommand(c){
  if(!c)return;
  const stick=shouldStick();
  appendTerminalHtml(`<span class="cmd">${esc(c)}</span><br>`,true);
  trimTerminal(stick);
  pendingTrimLeadingBlank=true;
  if(stick)scheduleStickScroll();
}
function transmitUserCommand(c,sensitive=false){
  if(!sensitive)echoCommand(c);
  const op=editorActive?'':c.split(/\s+/)[0];
  if(!sensitive && /^(?:n|s|e|w|ne|nw|se|sw|u|d|north|south|east|west|northeast|northwest|southeast|southwest|up|down|go|enter|out)$/i.test(op)){
    // Catworld-style feel: move the marker immediately using already-known topology;
    // the silent HUD response that follows is authoritative and corrects it if needed.
    optimisticMapMove(c);mapRefreshPending=true;
  }
  if(!sensitive&&['score','hp','skills','inventory','look','go'].includes(op))observer.begin(op,c);
  sessionRecorder.beginCommand(c,{sensitive});
  ws.send(c+'\r\n');
  if(editorActive&&(c.trim()==='.'||c.trim()==='~q'))editorActive=false;
  input.type='text';input.autocomplete='off';tail='';
}
function flushPendingUserCommands(){
  if(hudPollInFlight||ws?.readyState!==1||!pendingUserCommands.length)return;
  const queued=pendingUserCommands;
  pendingUserCommands=[];
  for(const item of queued)transmitUserCommand(item.command,item.sensitive);
}
// auto：觸發或計時器送出的指令。和玩家打的指令一樣顯示在畫面上（同樣顏色），
// 只是不會被當成密碼輸入（輸入密碼時本來就不會自動送）。
// 送到伺服器的方式和玩家打字完全相同，所以也會讓角色的發呆時間歸零。
// keepSpaces：留言／寫信的編輯器裡，每行前面的空白（半形、全形）是內容的一部分，不能去掉。
function send(c,{auto=false,keepSpaces=false}={}){
  const sensitive=!auto&&input.type==='password';
  c=keepSpaces?String(c??'').replace(/[\r\n]+/g,''):String(c??'').trim();
  if(c){lastUserCommandAt=Date.now();if(hudKickTimer){clearTimeout(hudKickTimer);hudKickTimer=null;}}
  if(ws?.readyState!==1){print('\n[尚未連上 ES2]\n',true);return;}
  // The silent HUD request and a real player command must never share the same
  // Neolith command/response window. If a click arrives while `webhud` is in
  // flight, hold it for a few milliseconds and send it immediately after the
  // @@WEBHUD|END marker. This prevents interaction buttons from being swallowed
  // by the HUD parser or interleaved with the telemetry command.
  if(hudPollInFlight){
    if(keepSpaces||!pendingUserCommands.some(x=>x.command===c&&x.sensitive===sensitive))
      pendingUserCommands.push({command:c,sensitive});
    // A HUD response is background telemetry. A real click must win quickly even
    // if the END marker is delayed/lost. Give the current HUD packet a very short
    // grace period, then release the queued player action.
    bgTimeout(()=>{
      if(!pendingUserCommands.length)return;
      if(hudPollInFlight)releaseHudPoll();
      flushPendingUserCommands();
    },HUD_ACTION_FLUSH_MS);
    return;
  }
  transmitUserCommand(c,sensitive);
}

// 觸發與計時器（設定存在瀏覽器）。輸入密碼、編輯文章或沒連線時不自動送指令。
const automation=new Automation({
  sendCommand:c=>send(c,{auto:true}),
  sendUserCommand:c=>send(c),
  notify:m=>print(`\n${m}\n`,true),
  isBlocked:()=>input.type==='password'||editorActive||ws?.readyState!==1,
  disconnect:()=>{
    if(reconnectTimer){clearBg(reconnectTimer);reconnectTimer=null;manualDisconnect=true;print('\n[已取消自動重連，輸入 #con 重新連線]\n',true);return true;}
    if(!ws||![WebSocket.OPEN,WebSocket.CONNECTING].includes(ws.readyState))return false;
    manualDisconnect=true;ws.close();return true;
  },
  connect:()=>{
    if(ws&&[WebSocket.OPEN,WebSocket.CONNECTING].includes(ws.readyState))return false;
    retry=0;connect();return true;
  },
});
setupAutomationUI(automation,{
  dialog:document.querySelector('#automationDialog'),
  toggleButton:document.querySelector('#automationToggle'),
  openButtons:[...document.querySelectorAll('[data-ui-automation]')].map(b=>[b,b.dataset.uiAutomation]),
});

// 打開網頁就直接連線（不再顯示封面）。
async function startGame(){
  if(started)return;
  started=true;gameApp.hidden=false;input.focus();
  keepPageAlive(); // 掛機：請瀏覽器不要凍結背景中的分頁
  await loadGraph();connect();
}
// Set once the box holds a "kept" command (see #form submit below) so a bare
// Enter repeats it. A plain input.select() only survives until the next
// click, because a mouse click on a focused field always collapses the
// selection to the click point first -- so typing right after clicking back
// into the box would insert into the kept text instead of replacing it
// (e.g. kept "w;e" + click + type "score" => "w;escore"). The mousedown
// handler below intercepts that click while armed and re-selects everything
// instead of letting the browser place a caret.
let repeatArmed=false;
input?.addEventListener('mousedown',e=>{
  if(repeatArmed){e.preventDefault();input.focus();input.select();}
});
input?.addEventListener('input',()=>{repeatArmed=false;historyPrefix=null;hi=history.length;});
document.querySelector('#form')?.addEventListener('submit',e=>{
  e.preventDefault();
  const raw=input.value;
  if(input.type==='password'){
    // Password entry never stays in the box or gets echoed/repeated.
    send(raw);input.value='';repeatArmed=false;input.focus();return;
  }
  if(editorActive){
    // 編輯器裡一行就是一行內容：保留前面的空白、不拆「;」、不留在欄位裡重複送。
    send(raw,{keepSpaces:true});input.value='';repeatArmed=false;input.focus();return;
  }
  // ';' chains several commands from one line, e.g. "e;e;e;n" walks east
  // three times then north. No inter-command delay unless the line uses
  // #wa (zMUD-style wait). At most 9 real commands per submit -- roughly what
  // a tick should hold; local # commands (#wa, #tr, #sh ...) don't count.
  // #3 e repeats, #dis/#con disconnect/reconnect -- see automation.js.
  rememberCommand(raw);hi=history.length;historyPrefix=null;
  automation.run(raw,'輸入欄',{user:true});
  // Keep the line in the box (selected, not cleared) so a bare Enter repeats
  // it -- handy for walking a corridor by holding Enter -- while typing
  // anything just overwrites the selection like normal.
  input.value=raw;input.focus();input.select();repeatArmed=true;
});

// Command focus hot-zone: clicking anywhere in the game window -- the nav
// rail, the console, the map/status/quick-action sidebar, the chat panel --
// returns focus to #command, so the player never has to click back into the
// input box just to keep typing. Real controls (buttons, links, other inputs,
// the settings dialog which lives outside .game-shell) keep their own click
// behaviour; only non-interactive space redirects focus.
// 地圖、角色狀態、聊天這些浮動視窗在 .game-shell 外面，也一起算進來。
function focusCommandFromClick(e){
  if(!input||input.disabled)return;
  const target=e.target instanceof Element?e.target:null;
  if(!target)return;
  if(target.closest('button,a,input,textarea,select,option,label,[contenteditable="true"],[role="button"]'))return;
  const selection=window.getSelection?.();
  if(selection&&String(selection).trim())return;
  try{input.focus({preventScroll:true});}catch(_){input.focus();}
}
for(const zone of document.querySelectorAll('.game-shell,#mapWindow,#statusWindow,#chatWindow'))zone.addEventListener('click',focusCommandFromClick);

// The padding/background of the command bar is part of the same hot-zone.  This
// explicit handler also covers browsers where a form/background click does not
// bubble as expected from nested layout elements.
document.querySelector('#form')?.addEventListener('click',e=>{
  if(!input||input.disabled)return;
  const target=e.target instanceof Element?e.target:null;
  if(target?.closest('button,input,textarea,select,option,label'))return;
  try{input.focus({preventScroll:true});}catch(_){input.focus();}
});
document.querySelectorAll('[data-cmd]').forEach(b=>b.addEventListener('click',()=>{send(b.dataset.cmd);input.focus();}));

// 介面設定：可拖曳的浮動視窗，不遮暗背景，邊調邊看得到訊息欄的效果；重新整理後不會自動打開。
setupFloatWindow({
  root:document.getElementById('uiSettings'),
  openButton:document.querySelector('[data-ui-settings]'),
  storageKey:'es2-settings-window-v1',rememberOpen:false,
  defaultGeometry:()=>({left:Math.max(16,(window.innerWidth-430)/2),top:Math.max(16,window.innerHeight*.12)}),
});
// 地圖視窗大小改變（拉角落、打開視窗）時重畫，讓看得到的範圍跟著視窗大小走。
{
  const mapEl=document.querySelector('#localMap');let mapResizeFrame=0;
  if(mapEl&&typeof ResizeObserver==='function')new ResizeObserver(()=>{
    if(mapResizeFrame)return;
    mapResizeFrame=requestAnimationFrame(()=>{mapResizeFrame=0;renderLocalMap();});
  }).observe(mapEl);
}
document.querySelectorAll('[data-map-action]').forEach(b=>b.addEventListener('click',()=>{
  const a=b.dataset.mapAction;if(a==='in')setMapZoom(mapZoom+.12);else if(a==='out')setMapZoom(mapZoom-.12);else setMapZoom(1);
}));
setupTextColor(document.getElementById('textColorSettings'));
const fontRange=document.querySelector('#terminalFontSize'),fontValue=document.querySelector('#terminalFontValue');
fontRange?.addEventListener('input',()=>{document.documentElement.style.setProperty('--terminal-font-size',fontRange.value+'px');sideSplitter.refresh();if(fontValue)fontValue.textContent=fontRange.value+'px';localStorage.setItem('es2-ui-font',fontRange.value);});
try{const f=localStorage.getItem('es2-ui-font');if(f&&fontRange){fontRange.value=f;document.documentElement.style.setProperty('--terminal-font-size',f+'px');sideSplitter.refresh();if(fontValue)fontValue.textContent=f+'px';}}catch(_){}



async function bootPreviewMode(){
  const p=new URLSearchParams(location.search);if(!p.has('preview'))return false;
  started=true;gameApp.hidden=false;conn.textContent='PREVIEW';
  graph={nodes:[
    {id:'/preview/main',label:'中央大街'},{id:'/preview/wuguan',label:'武館'},{id:'/preview/yaopu',label:'藥舖'},{id:'/preview/qianzhuang',label:'錢莊'},{id:'/preview/kelou',label:'客棧'},{id:'/preview/nanjie',label:'南街'},{id:'/preview/xiaoxiang',label:'小巷'},{id:'/preview/caopeng',label:'草棚'},{id:'/preview/yingdi',label:'振武軍營'}
  ],edges:[]};indexGraph();
  const es=[['/preview/main','north','/preview/wuguan'],['/preview/wuguan','north','/preview/yaopu'],['/preview/main','east','/preview/qianzhuang'],['/preview/main','west','/preview/kelou'],['/preview/main','south','/preview/nanjie'],['/preview/qianzhuang','south','/preview/xiaoxiang'],['/preview/xiaoxiang','east','/preview/caopeng'],['/preview/caopeng','east','/preview/yingdi']];
  for(const [a,d,b] of es){mergeRuntimeExit(a,d,b,nodeById.get(b)?.label||'');const rev={north:'south',south:'north',east:'west',west:'east'}[d];mergeRuntimeExit(b,rev,a,nodeById.get(a)?.label||'');}
  setRoomMapMeta('/preview/main','雪亭','地面','');setExactRoom('/preview/main','中央大街');
  observer.state.vitals={hp:{current:176,limit:210},jing:{current:1296,limit:1296},qi:{current:1738,limit:1738},shen:{current:449,limit:449},food:{current:303,limit:551},water:{current:261,limit:429},fatigue:{current:0,limit:100}};observer.state.score={level:15};renderObserved();
  term.innerHTML='<span class="room-line">【雪亭・中央大街】</span><br>這是一條寬闊的青石大街，兩旁店舖林立，遠處可見城門與屋脊。<br><br><span class="exit-line">出口：北、南、東、西</span><br><br>一名往來的行人從街角走過。<br><br><span class="cmd">&gt; look</span><br>你仔細觀察四周，風裡帶著炊煙與木香。';
  chatWindow.add('chat','【閒聊】測試玩家﹕新版介面預覽。');
  return true;
}
bootPreviewMode().then(()=>startGame());

function historyMatch(cmd){return cmd.toLowerCase().startsWith(historyPrefix.toLowerCase());}
function showHistory(value){input.value=value;repeatArmed=false;const n=value.length;input.setSelectionRange(n,n);}
// Tab：插入空白到下一個 8 格的位置（中文字算 2 格），不會跳離輸入列。
function textColumns(t){let w=0;for(const ch of t)w+=/[\u1100-\u115f\u2e80-\ua4cf\uac00-\ud7a3\uf900-\ufaff\ufe30-\ufe4f\uff00-\uff60\uffe0-\uffe6]|[\u{20000}-\u{3fffd}]/u.test(ch)?2:1;return w;}
function insertTab(){
  const start=input.selectionStart??input.value.length,end=input.selectionEnd??start;
  const col=textColumns(input.value.slice(0,start));
  const pad=' '.repeat(8-col%8);
  input.value=input.value.slice(0,start)+pad+input.value.slice(end);
  const pos=start+pad.length;input.setSelectionRange(pos,pos);
  repeatArmed=false;historyPrefix=null;hi=history.length;
}
// 輸入列按 PageUp／PageDown 翻訊息欄；End 照常把游標移到文字最後面，0.5 秒內連按兩下才回到最新訊息並繼續跟著捲動。
function pageTerminal(dir){
  const line=parseFloat(getComputedStyle(term).lineHeight)||24;
  term.scrollTop+=dir*Math.max(line,term.clientHeight-line*2);
}
let lastEndPress=0;
function followLatest(){stickToBottom=true;term.scrollTop=term.scrollHeight;autoScrollTop=term.scrollTop;updateScrollLatest();}
input?.addEventListener('keydown',e=>{
  if(!e.shiftKey&&!e.ctrlKey&&!e.altKey&&!e.metaKey&&!e.isComposing){
    if(e.key==='PageUp'){e.preventDefault();pageTerminal(-1);return;}
    if(e.key==='PageDown'){e.preventDefault();pageTerminal(1);return;}
    if(e.key==='End'&&!e.repeat){
      const now=Date.now();
      if(now-lastEndPress<=500){lastEndPress=0;followLatest();}
      else lastEndPress=now;
      return;
    }
  }
  if(e.key==='Tab'&&!e.shiftKey&&!e.ctrlKey&&!e.altKey&&!e.metaKey&&!e.isComposing){
    e.preventDefault();insertTab();return;
  }
  if(e.key==='ArrowUp'){
    e.preventDefault();
    if(historyPrefix===null){
      // 剛送出、指令還反白留在欄位時，視為沒有輸入，從頭翻。
      historyPrefix=repeatArmed?'':input.value;hi=history.length;
    }
    for(let i=hi-1;i>=0;i--)if(historyMatch(history[i])){hi=i;showHistory(history[i]);return;}
  }
  else if(e.key==='ArrowDown'){
    e.preventDefault();
    if(historyPrefix===null)return;
    for(let i=hi+1;i<history.length;i++)if(historyMatch(history[i])){hi=i;showHistory(history[i]);return;}
    // 翻到底：回到當初輸入的文字。
    hi=history.length;showHistory(historyPrefix);historyPrefix=null;
  }
});

// 訊息欄每一行的行頭（第一個字左邊的空白處）按一下：反白整行並複製。
function copyText(text){
  if(navigator.clipboard?.writeText)return navigator.clipboard.writeText(text).catch(()=>document.execCommand('copy'));
  try{document.execCommand('copy');}catch(_){}
}
function inLineGutter(e){
  if(e.target!==term)return false;
  const rect=term.getBoundingClientRect();
  return e.clientX<rect.left+(parseFloat(getComputedStyle(term).paddingLeft)||0);
}
term?.addEventListener('mousemove',e=>term.classList.toggle('line-pick',inLineGutter(e)));
term?.addEventListener('mouseleave',()=>term.classList.remove('line-pick'));
term?.addEventListener('mousedown',e=>{
  if(e.button!==0||e.target!==term)return;
  const rect=term.getBoundingClientRect();
  const padLeft=parseFloat(getComputedStyle(term).paddingLeft)||0;
  if(e.clientX>=rect.left+padLeft)return;
  const x=rect.left+padLeft+2,y=e.clientY;
  let node=null,offset=0;
  if(document.caretPositionFromPoint){const p=document.caretPositionFromPoint(x,y);if(p){node=p.offsetNode;offset=p.offset;}}
  else if(document.caretRangeFromPoint){const r=document.caretRangeFromPoint(x,y);if(r){node=r.startContainer;offset=r.startOffset;}}
  if(!node||!term.contains(node))return;
  e.preventDefault();
  const sel=window.getSelection();
  sel.removeAllRanges();
  const r=document.createRange();r.setStart(node,offset);r.collapse(true);sel.addRange(r);
  sel.modify('move','backward','lineboundary');
  sel.modify('extend','forward','lineboundary');
  const text=sel.toString().replace(/\n+$/,'');
  if(text.trim())copyText(text);
});

// 訊息欄往上翻時出現「▼」按鈕，按下直接捲到最新的訊息。
const scrollLatest=document.querySelector('#scrollLatest');
function updateScrollLatest(){if(scrollLatest)scrollLatest.hidden=stickToBottom;}
term?.addEventListener('scroll',()=>{
  // 自動捲到底後、捲動事件送達前可能又有新文字進來；位置沒變就視為自己的捲動，維持貼底。
  if(term.scrollTop!==autoScrollTop)stickToBottom=isNearBottom();
  updateScrollLatest();
});
scrollLatest?.addEventListener('click',e=>{e.preventDefault();e.stopPropagation();followLatest();input?.focus();});
