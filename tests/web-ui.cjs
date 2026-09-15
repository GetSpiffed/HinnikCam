const assert = require('node:assert/strict');
const fs = require('node:fs');
const vm = require('node:vm');
const html = fs.readFileSync('include/web_page.h','utf8');
const source = html.split('<script>')[1].split('</script>')[0];
async function fixture({confirmed=true,shutdownOK=true,captureOK=true,offline=false}={}) {
  const elements=Object.fromEntries([...html.matchAll(/id="([^"]+)"/g)].map(m=>[m[1],{
    textContent:'',disabled:false,hidden:true,dataset:{},removeAttribute(k){delete this[k];}
  }]));
  const calls=[],downloads=[];
  const context=vm.createContext({
    document:{getElementById:id=>elements[id],body:{appendChild(){}},
      createElement:()=>({click(){downloads.push(this.download);},remove(){}})},
    confirm:()=>confirmed, AbortController, Date,
    URL:{createObjectURL:()=> 'blob:test',revokeObjectURL(){}},
    setTimeout:()=>1,clearTimeout:()=>{},
    fetch:async(url,options)=>{
      calls.push({url,options});
      if(url==='/status' && offline)throw Error('Offline');
      return {ok:url==='/shutdown'?shutdownOK:url==='/capture'?captureOK:true,
        blob:async()=>({type:'image/jpeg',size:1024}),json:async()=>({
          camera_ready:true,streaming:true,frame_age_ms:100,clients:1,
          ip:'192.168.4.1',power_ready:true,usb:true,battery:true,
          charging:true,battery_mv:3980,shutting_down:false
        })};
    }
  });
  vm.runInContext(source,context);
  await new Promise(setImmediate);
  return {elements,calls,downloads};
}
(async()=>{
  for(const confirmed of [false,true]){
    const f=await fixture({confirmed});
    assert.match(f.elements.battery.textContent,/3.98 V/);
    await f.elements.shutdown.onclick();
    const calls=f.calls.filter(c=>c.url==='/shutdown');
    assert.equal(calls.length,confirmed?1:0);
    if(confirmed){
      assert.equal(calls[0].options.method,'POST');
      assert.equal(calls[0].options.headers['X-HinnikCam-Confirm'],'yes');
      assert.equal(f.elements.capture.disabled,true);
      assert.match(f.elements.status.textContent,/schakelt uit/);
    }
  }
  let f=await fixture({shutdownOK:false});
  await f.elements.shutdown.onclick();
  assert.equal(f.elements.shutdown.disabled,false);
  assert.match(f.elements.status.textContent,/niet bevestigd/);
  f=await fixture();
  await f.elements.capture.onclick();
  assert.equal(f.downloads.length,1);
  assert.match(f.downloads[0],/^HinnikCam-.*.jpg$/);
  assert.equal(f.elements['photo-open'].hidden,false);
  assert.equal(f.elements.capture.disabled,false);
  f=await fixture({captureOK:false});
  await f.elements.capture.onclick();
  assert.equal(f.downloads.length,0);
  assert.match(f.elements['photo-status'].textContent,/niet opgehaald/);
  assert.equal(f.elements.capture.disabled,false);
  f=await fixture({offline:true});
  assert.equal(f.elements.capture.disabled,true);
  await f.elements.capture.onclick();
  assert.equal(f.downloads.length,0);
  console.log('PASS: shutdown cancel/confirm/failure; JPEG download/failure; offline; battery');
})().catch(e=>{console.error(e);process.exitCode=1;});