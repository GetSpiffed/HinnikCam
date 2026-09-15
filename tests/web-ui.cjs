const assert = require('node:assert/strict');
const fs = require('node:fs');
const vm = require('node:vm');
const source = fs.readFileSync('include/web_page.h', 'utf8').split('<script>')[1].split('</script>')[0];
async function run(confirmResult, succeeds) {
  const elements = Object.fromEntries(['camera','status','restart','shutdown','power','info'].map(id =>
    [id, {textContent:'', disabled:false, removeAttribute(k){ delete this[k]; }}]));
  const calls = [];
  const context = vm.createContext({
    document:{getElementById:id=>elements[id]},
    confirm:()=>confirmResult,
    AbortController,
    setTimeout:()=>1, clearTimeout:()=>{},
    fetch: async (url, options) => {
      calls.push({url, options});
      return {ok:url==='/status' || succeeds, json:async()=>({
        camera_ready:true, streaming:false, frame_age_ms:10000,
        clients:1, ip:'192.168.4.1', power_ready:true, usb:true,
        battery:true, charging:true, battery_mv:3980, shutting_down:false
      })};
    }
  });
  vm.runInContext(source, context);
  await new Promise(setImmediate);
  assert.match(elements.power.textContent, /3.98 V/);
  await elements.shutdown.onclick();
  const shutdownCalls = calls.filter(c=>c.url==='/shutdown');
  assert.equal(shutdownCalls.length, confirmResult ? 1 : 0);
  if (confirmResult) {
    assert.equal(shutdownCalls[0].options.method,'POST');
    assert.equal(shutdownCalls[0].options.headers['X-HinnikCam-Confirm'],'yes');
    assert.equal(elements.shutdown.disabled,succeeds);
    assert.equal(elements.restart.disabled,succeeds);
    if (succeeds) assert.match(elements.status.textContent,/schakelt uit/);
    else assert.match(elements.status.textContent,/niet bevestigd/);
  }
}
(async()=>{
  await run(false,true);
  await run(true,true);
  await run(true,false);
  console.log('PASS: voltage display, cancelled shutdown, confirmed POST, failed shutdown retry');
})().catch(e=>{console.error(e);process.exitCode=1;});