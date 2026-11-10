// zMUD／Telnet 直連入口測試：node tests/test_direct_zmud.mjs
import net from 'node:net';
import assert from 'node:assert/strict';
import iconv from '../server/node_modules/iconv-lite/lib/index.js';
import {startDirectListener, TelnetSplitter} from '../server/direct.js';

const wait = ms => new Promise(r => setTimeout(r, ms));
const IAC = 255, WILL = 251, ECHO = 1;
const received = [];
const mud = net.createServer(s => {
  s.on('data', d => received.push(d));
  const text = Buffer.from('歡迎來到天朝！■□ ✦\r\n', 'utf8');   // ✦ 是 Big5 沒有的字
  s.write(Buffer.from([IAC, WILL, ECHO]));
  s.write(text.subarray(0, 4));     // 切在中文字中間
  setTimeout(() => s.write(text.subarray(4)), 30);
});
await new Promise(r => mud.listen(0, '127.0.0.1', r));
const mudPort = mud.address().port;
const common = {host: '127.0.0.1', mudHost: '127.0.0.1', mudPort, maxSessions: 1, connectTimeoutMs: 2000, bridgeSecret: '', trace: () => {}};
const free = async () => { const s = net.createServer(); await new Promise(r => s.listen(0, '127.0.0.1', r)); const p = s.address().port; s.close(); return p; };
const zport = await free(), tport = await free();
const z = startDirectListener({...common, name: 'zmud', port: zport, big5: true});
const t = startDirectListener({...common, name: 'telnet', port: tport, big5: false});
await wait(100);

const connect = port => new Promise(res => { const c = net.createConnection(port, '127.0.0.1', () => { const chunks = []; c.on('data', d => chunks.push(d)); res({c, get: () => Buffer.concat(chunks)}); }); });

// zMUD：收到 Big5，控制訊號原樣；送出 Big5 遊戲端收到 UTF-8
const zc = await connect(zport);
await wait(200);
const got = zc.get();
assert.deepEqual([...got.subarray(0, 3)], [IAC, WILL, ECHO], 'IAC 原樣');
assert.equal(iconv.decode(got.subarray(3), 'cp950'), '歡迎來到天朝！■□ ?\r\n', 'Big5 轉碼、沒有的字變 ?');
zc.c.write(Buffer.concat([iconv.encode('say 你好', 'cp950').subarray(0, 7), Buffer.alloc(0)]));   // 切在「你」字中間（say␠ + 1 byte）
await wait(30);
zc.c.write(iconv.encode('say 你好', 'cp950').subarray(7));
zc.c.write('\r\n');
await wait(100);
assert.equal(Buffer.concat(received).toString('utf8'), 'say 你好\r\n', '玩家 Big5 → 遊戲 UTF-8');

// 人數上限
const over = await connect(zport);
await wait(100);
assert.match(iconv.decode(over.get(), 'cp950'), /人數已滿/);
zc.c.destroy(); await wait(100);

// Telnet：UTF-8 原樣
received.length = 0;
const tc = await connect(tport);
await wait(200);
assert.equal(tc.get().subarray(3).toString('utf8'), '歡迎來到天朝！■□ ✦\r\n');
tc.c.destroy();

// 控制訊號跨封包、子協商
const out = [];
const sp = new TelnetSplitter(x => out.push(['t', x.toString()]), x => out.push(['r', [...x]]));
sp.feed(Buffer.from([65, IAC])); sp.feed(Buffer.from([250, 24, 1, IAC])); sp.feed(Buffer.from([240, 66]));
assert.deepEqual(out.filter(x => x[0] === 't'), [['t', 'A'], ['t', 'B']]);
assert.deepEqual(out.filter(x => x[0] === 'r').flatMap(x => x[1]), [IAC, 250, 24, 1, IAC, 240]);

// 遊戲未啟動
mud.close();
const dead = startDirectListener({...common, mudPort: await free(), name: 'zmud', port: await free(), big5: true});
await wait(50);
const dc = await connect(dead.address().port);
await wait(300);
assert.match(iconv.decode(dc.get(), 'cp950'), /無法連線/);

for (const s of [z, t, dead]) s.close();
console.log('PASS test_direct_zmud');
process.exit(0);
