#pragma once
#include <Arduino.h>
const char WEB_PAGE[] PROGMEM = R"HTML(<!doctype html>
<html lang="nl"><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>HinnikCam</title>
<style>
body{margin:20px auto;padding:0 12px;max-width:800px;background:#18201b;color:#eef5ef;font:17px system-ui}
img{display:block;width:100%;aspect-ratio:4/3;object-fit:contain;background:#080b09}
button,a{font:inherit;color:inherit}button{padding:10px;background:#36553e;border:1px solid #89a98e;border-radius:6px}
</style>
<h1>HinnikCam</h1><p id="status" role="status">Verbinding controleren...</p>
<img id="camera" alt="Live camera in de trailer">
<p><button id="restart">Stream opnieuw starten</button> <a href="/stream">Directe stream</a></p>
<p id="info"></p><p>Bij stilstaand beeld: start de stream opnieuw. Geen internet nodig.</p>
<script>
const camera=document.getElementById('camera'), status=document.getElementById('status');
function start(){camera.src='/stream?t='+Date.now();}
camera.onerror=()=>{status.textContent='Stream onderbroken; start opnieuw.';};
document.getElementById('restart').onclick=start;
async function poll(){
  const controller=new AbortController(), timer=setTimeout(()=>controller.abort(),2500);
  try {
    const r=await fetch('/status',{cache:'no-store',signal:controller.signal});
    if(!r.ok) throw Error();
    const s=await r.json();
    status.textContent=!s.camera_ready?'Camerafout: controleer Serial en herstart het board.':
      s.streaming && s.frame_age_ms<3000?'Verbonden - camera verstuurt beelden.':'Verbonden - geen recente streambeelden.';
    document.getElementById('info').textContent='AP actief | '+s.ip+' | Wifi-clients: '+s.clients+' | VGA 640x480 JPEG';
  } catch {status.textContent='Geen verbinding met HinnikCam.';}
  finally {clearTimeout(timer);setTimeout(poll,2000);}
}
start();poll();
</script></html>)HTML";
