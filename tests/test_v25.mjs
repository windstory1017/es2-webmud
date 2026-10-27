import assert from 'node:assert/strict';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import {execFileSync,spawnSync} from 'node:child_process';

const root=new URL('../',import.meta.url).pathname;
const tool=n=>path.join(root,'tools',n);
const dir=fs.mkdtempSync(path.join(os.tmpdir(),'es2-v25-'));
// Every artifact is bound to one validation campaign (tools added --campaign after v2.5).
const campaign=path.join(dir,'campaign.json');
fs.writeFileSync(campaign,JSON.stringify({
 format:'es2-validation-campaign-v1',
 campaignId:'campaign-v25',
 projectVersion:'2.5.0',
 source:{commit:'a'.repeat(40),mudlibTreeSha256:'b'.repeat(64)}
}));

// 1. Build a complete captured-session package.
const session=path.join(dir,'session.jsonl');
const rows=[
 {command:'score',response:'目前是等級 3\n累積業力共 10 點\n'},
 {command:'hp',response:'形體 10/10    精 10/10    氣 10/10    神 10/10\n食物 10/10    飲水 10/10    疲勞 0/10\n'},
 {command:'skills',response:'目前並沒有學會任何技能。\n'},
 {command:'inventory',response:'目前你身上沒有任何東西。\n'},
 {command:'look',response:'你攻擊對手。\n你的眼前一黑，接著什麼也不知道了....\n慢慢地你終於又有了知覺....\n你死了！！\n重新連線完畢。\n'},
 {command:'go north',response:'北方景象。\n'},
 // Movement now needs a captured look after the go (verify_movement_transition.mjs).
 {command:'look',response:'北邊的小路\n'}
];
fs.writeFileSync(session,rows.map(x=>JSON.stringify(x)).join('\n'));
const sessionPkg=path.join(dir,'session-package');
execFileSync('node',[tool('build_session_package.mjs'),'--session',session,'--out',sessionPkg,'--campaign',campaign],{stdio:'pipe'});
let v=JSON.parse(execFileSync('node',[tool('validate_session_package.mjs'),sessionPkg],{encoding:'utf8'}));
assert.equal(v.ok,true);assert.equal(v.acceptanceComplete,true);

// Tamper detection.
const pkgSession=path.join(sessionPkg,'session.jsonl');
const original=fs.readFileSync(pkgSession,'utf8');
fs.writeFileSync(pkgSession,original.replace('北方景象','竄改景象'));
let bad=spawnSync('node',[tool('validate_session_package.mjs'),sessionPkg],{encoding:'utf8'});
assert.notEqual(bad.status,0);
fs.writeFileSync(pkgSession,original);

// 2. Build evidence-backed multiplayer plan.
const plan=path.join(dir,'multiplayer.json');
execFileSync('node',[tool('new_multiplayer_test_plan.mjs'),'--out',plan,'--campaign',campaign],{stdio:'pipe'});
const ids=['bridge-10','bridge-50','login-isolation','same-room','chat','movement-isolation','combat-isolation','disconnect-one','reconnect'];
for(const id of ids){
 const ev=path.join(dir,`${id}.txt`);fs.writeFileSync(ev,`live evidence for ${id}\n`);
 execFileSync('node',[tool('attach_multiplayer_evidence.mjs'),'--plan',plan,'--stage',id,'--evidence',ev],{stdio:'pipe'});
}
v=JSON.parse(execFileSync('node',[tool('validate_multiplayer_plan.mjs'),'--plan',plan],{encoding:'utf8'}));
assert.equal(v.ok,true);

// 3. Build + hash-validate runtime evidence bundle.
// Contents must pass the semantic checks in validate_runtime_evidence.py.
const evidence={
 'source-status.json':JSON.stringify({ok:true}),
 'runtime-smoke.json':JSON.stringify({ok:true}),
 'login.txt':'您的使用者代號：citest\n請輸入密碼:\n',
 'movement.txt':'look\n客棧大堂\ngo north\n北邊的小路\n',
 'combat.txt':'你攻擊對手。\n對手向你攻擊。\n你的眼前一黑。\n',
 'reconnect.txt':'重新連線完畢。\n'
};
const evNames=Object.keys(evidence);
for(const n of evNames)fs.writeFileSync(path.join(dir,n),evidence[n]);
const bundle=path.join(dir,'runtime-bundle');
execFileSync('python',[tool('evidence_bundle.py'),bundle,'--campaign',campaign,
 '--source-status',path.join(dir,evNames[0]),'--runtime-smoke',path.join(dir,evNames[1]),
 '--login-transcript',path.join(dir,evNames[2]),'--movement-transcript',path.join(dir,evNames[3]),
 '--combat-transcript',path.join(dir,evNames[4]),'--reconnect-transcript',path.join(dir,evNames[5])],{stdio:'pipe'});
v=JSON.parse(execFileSync('python',[tool('validate_runtime_evidence.py'),bundle],{encoding:'utf8'}));
assert.equal(v.ok,true);
const tamper=path.join(bundle,'combat.txt');const combatOriginal=fs.readFileSync(tamper,'utf8');fs.appendFileSync(tamper,'tamper');
bad=spawnSync('python',[tool('validate_runtime_evidence.py'),bundle],{encoding:'utf8'});assert.notEqual(bad.status,0);fs.writeFileSync(tamper,combatOriginal);

// 4. End-to-end final campaign gate.
const rawRelease=path.join(dir,'release-raw.json');fs.writeFileSync(rawRelease,JSON.stringify({ok:true,version:'2.5.0'}));
const release=path.join(dir,'release.json');
execFileSync('python',[tool('bind_release_report.py'),'--campaign',campaign,'--release',rawRelease,'--out',release],{stdio:'pipe'});
const final=path.join(dir,'final.json');
execFileSync('python',[tool('final_campaign_gate.py'),'--campaign',campaign,'--release',release,'--runtime-bundle',bundle,'--session-package',sessionPkg,'--multiplayer',plan,'--out',final],{stdio:'pipe'});
const finalReport=JSON.parse(fs.readFileSync(final,'utf8'));assert.equal(finalReport.ok,true);
console.log('v2.5 full validation campaign tests passed');
