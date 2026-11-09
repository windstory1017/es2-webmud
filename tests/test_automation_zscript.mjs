// 網頁 zMUD 指令（docs/WEB_ZMUD_COMMANDS.md）：#if、#var、#alias、#loop、#while、#case、#math、#add、
// #abort、#trigger、#alarm、快速移動、函數、#gag、#color。
import assert from 'node:assert/strict';
globalThis.localStorage={store:{},getItem(k){return this.store[k]??null;},setItem(k,v){this.store[k]=String(v);}};
const {Automation,splitCommandLine,colorCode}=await import('../web/automation.js');
const {splitStatements,evaluate,expandSpeedwalk,expandText}=await import('../web/zscript.js');
const sleep=ms=>new Promise(r=>setTimeout(r,ms));
const clean=s=>String(s).replace(/\x1b\[[0-9;]*m/g,'');

function make(status={}){
  localStorage.store={};
  const log=[],notes=[];let beeps=0;
  const a=new Automation({
    sendCommand:c=>log.push(c),sendUserCommand:c=>log.push(c),notify:m=>notes.push(m),
    isBlocked:()=>false,getStatus:()=>status,beep:()=>{beeps++;},
  });
  return {a,log,notes,beeps:()=>beeps};
}
const ctx={getVar:n=>({x:5,name:'野狗'}[n]??''),hasVar:n=>['x','name'].includes(n)};

// 拆指令：大括號、小括號裡的 ; 不拆；#3 {…} 展開
assert.deepEqual(splitStatements('a;#if (1) {b;c} {d};e'),['a','#if (1) {b;c} {d}','e']);
assert.deepEqual(splitCommandLine('#2 {n;e};s'),['n','e','n','e','s']);
assert.deepEqual(splitCommandLine('#3 e;n; ;#2 #wa 10'),['e','e','e','n','#wa 10','#wa 10']);

// 運算式
assert.equal(evaluate('1+2*3',ctx),7);
assert.equal(evaluate('(1+2)*3',ctx),9);
assert.equal(evaluate('7/2',ctx),3);                 // 除法取整數
assert.equal(evaluate('7%3',ctx),1);
assert.equal(evaluate('@x > 3 & @x < 10',ctx),1);
assert.equal(evaluate('@x = 4 | !0',ctx),1);
assert.equal(evaluate('@name = 野狗',ctx),1);
assert.equal(evaluate('"測試 玩家" = 測試 玩家',ctx),1); // 沒加引號的相鄰文字併成一段
assert.equal(evaluate('10 >= 9',ctx),1);
assert.equal(evaluate('"abc" < "abd"',ctx),1);
assert.equal(evaluate('-3+1',ctx),-2);
assert.equal(evaluate('%len(@name)+%pos(狗,@name)',ctx),4);
assert.equal(evaluate('%if(@x>1,大,小)',ctx),'大');

// 文字展開：變數、函數、~ 跳脫、不認識的 @ 保留
assert.equal(expandText('kill @name',ctx),'kill 野狗');
assert.equal(expandText('say %left(abcdef,2)%right(abcdef,2) %eval(@x*2)',ctx),'say abef 10');
assert.equal(expandText('say a@b.com ~@x',ctx),'say a@b.com @x');
assert.match(expandText('%time',ctx),/^\d\d:\d\d:\d\d$/);
for(let i=0;i<50;i++){const r=Number(expandText('%random(1,6)',ctx));assert.ok(r>=1&&r<=6);}

// 快速移動
assert.deepEqual(expandSpeedwalk('.3n2e'),['n','n','n','e','e']);
assert.deepEqual(expandSpeedwalk('.2(ne)su'),['ne','ne','s','u']);
assert.equal(expandSpeedwalk('.hello'),null);
{const {a,log}=make();a.run('.2nw;look',null,{user:true});assert.deepEqual(log,['n','n','w','look']);}

// #if／else、角色狀態變數
{const {a,log}=make({hp:{current:30,max:100},qi:{current:500,max:800}});
 a.run('#if (@hp < 50) {eat;drink} {say ok}');
 a.run('#if (@qi*2 > @qimax) {exert} {rest}');
 a.run('#if (@hp > 50) {never}');
 assert.deepEqual(log,['eat','drink','exert']);}

// #var、@變數、#unvar、角色狀態不能設定
{const {a,log,notes}=make({hp:{current:1,max:2}});
 a.run('#var target 野狗;kill @target;#var msg {你好 世界};say @msg');
 assert.deepEqual(log,['kill 野狗','say 你好 世界']);
 assert.equal(a.state.vars.target,'野狗');
 a.run('#var hp 5');assert.ok(notes.some(n=>n.includes('角色狀態')));
 a.run('#unvar target');assert.equal(a.state.vars.target,undefined);
 assert.ok(JSON.parse(localStorage.store['es2-automation-v1']).vars.msg==='你好 世界');}

// #math、#add
{const {a,log}=make();
 a.run('#var a 3;#math b @a*4+1;#add b 2;#add c;say @b @c');
 assert.deepEqual(log,['say 15 1']);}

// #alias：%1、%*、沒有 % 時接在後面、大小寫不分、#unalias
{const {a,log}=make();
 a.run('#alias kk {kill %1;perform %1};#alias tt {tell %1 %*};#alias gg get all');
 a.run('kk 野狗;TT alice 晚安 你好;gg from corpse');
 assert.deepEqual(log,['kill 野狗','perform 野狗','tell alice alice 晚安 你好','get all from corpse']);
 a.run('#unalias kk;kk 狼');assert.equal(log.at(-1),'kk 狼');}

// #loop、%i、#case、#abort
{const {a,log}=make();
 a.run('#loop 3 {get coin};#loop 1,3 {say %i};#case 2 {一} {二} {三};#case 9 {x}');
 a.run('say a;#abort;say b');
 assert.deepEqual(log,['get coin','get coin','get coin','say 1','say 2','say 3','二','say a']);}

// #while 與 #add 計數；停不下來的 #while 會被擋下
{const {a,log,notes}=make();
 a.run('#var n 0;#while (@n < 3) {#add n 1;say @n}');
 assert.deepEqual(log,['say 1','say 2','say 3']);
 a.run('#while (1) {#add spin 1}');
 assert.ok(notes.some(n=>n.includes('重複太多次')));}

// 玩家輸入欄一次最多送 9 個指令（迴圈展開的也算）
{const {a,log,notes}=make();
 a.run('#loop 20 {e}',null,{user:true});
 assert.equal(log.length,9);assert.ok(notes.includes('你的動作太快了...'));}

// #trigger／#untrigger；#tr on|off 仍是開關全部觸發
{const {a,log}=make();
 a.run('#trigger {^(%w)向你撲過來} {kill %1} 戰鬥');
 assert.equal(a.state.triggers.length,1);assert.equal(a.state.triggers[0].cls,'戰鬥');
 a.processOutput('野狗向你撲過來\n',clean);
 assert.deepEqual(log,['kill 野狗']);
 a.run('#tr off');assert.equal(a.state.triggersEnabled,false);a.run('#tr on');
 a.run('#untrigger {^(%w)向你撲過來}');assert.equal(a.state.triggers.length,0);}

// #alarm 幾秒後執行一次
{const {a,log}=make();
 a.run('#alarm 0.05 {eat}');assert.deepEqual(log,[]);
 await sleep(120);assert.deepEqual(log,['eat']);}

// #echo、#beep
{const {a,notes,beeps}=make();a.run('#echo 哈囉 @none;#beep');assert.ok(notes.includes('哈囉 @none'));assert.equal(beeps(),1);}

// #gag：規則隱藏整行；觸發裡單獨的 #gag 隱藏觸發的那一行
{const {a,log}=make();
 a.run('#gag {^【謠言】}');
 assert.equal(a.processOutput('甲\n【謠言】某人說話\n乙\n',clean),'甲\n乙\n');
 a.run('#trigger {^你覺得餓了} {#gag;eat}');
 assert.equal(a.processOutput('你覺得餓了。\n丙\n',clean),'丙\n');
 assert.deepEqual(log,['eat']);
 a.run('#ungag {^【謠言】}');assert.equal(a.processOutput('【謠言】x\n',clean),'【謠言】x\n');}

// #color：只替符合的文字上色，原本的顏色在後面補回
{const {a}=make();
 assert.equal(colorCode('亮黃'),'93');assert.equal(colorCode('hi red'),'91');assert.equal(colorCode('橘'),'38;5;208');assert.equal(colorCode('彩虹'),null);
 a.run('#color {告訴你} 亮黃');
 const out=a.processOutput('\x1b[32m阿明告訴你：嗨\x1b[0m\n',clean);
 assert.equal(clean(out),'阿明告訴你：嗨\n');
 assert.ok(out.includes('\x1b[93m告訴你\x1b[0m\x1b[32m'));
 a.run('#uncolor {告訴你}');assert.equal(a.processOutput('告訴你\n',clean),'告訴你\n');}

// 匯出、匯入帶著縮寫與變數；舊版匯出檔不會清掉它們
{const {a}=make();
 a.run('#alias kk {kill %1};#var x 1;#gag {xx};#color {yy} 紅');
 const json=a.exportJSON();
 const {a:b}=make();b.importJSON(json);
 assert.equal(b.state.aliases.kk,'kill %1');assert.equal(b.state.vars.x,1);
 assert.equal(b.state.gags.length,1);assert.equal(b.state.colors[0].color,'紅');
 b.importJSON(JSON.stringify({triggers:[],timers:[]}));
 assert.equal(b.state.aliases.kk,'kill %1');}

console.log('automation zscript tests passed');
