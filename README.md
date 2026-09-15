# HinnikCam

Een minimale, zelfstandige paardentrailercamera: **telefoon → HinnikCam-wifi → browser → live beeld**. Geen router, cloud of internet nodig tijdens gebruik.

## Hardware en basis

- LilyGO **T-Camera S3**, ESP32-S3FN16R8, OV2640, 16 MB flash, 8 MB OPI-PSRAM en AXP2101. Dit project gaat uit van de uitvoering met PIR/OLED (`LILYGO_ESP32S3_CAM_PIR_VOICE`), niet de T-Camera **Plus** S3 of SIM-variant.
- Begin met stabiele USB-voeding en een USB-datakabel.
- PlatformIO + Arduino; `espressif32@7.1.3` levert Arduino-ESP32 2.0.17. Deze core is bewust gekozen omdat LilyGO die voor dit board aanbeveelt. Geen overstap naar Arduino 3.x nodig voor deze eerste versie.
- QIO-flash op 80 MHz en OPI-PSRAM zijn expliciet ingesteld. Camera-init stopt met een duidelijke fout als PSRAM ontbreekt.
- OV2640: VGA (640×480), JPEG-kwaliteit 12, 20 MHz XCLK, twee framebuffers in PSRAM en `CAMERA_GRAB_LATEST`. Verzenden is begrensd op circa 12 fps; werkelijke snelheid hangt af van licht en wifi.

Eerst is de lege repository gecontroleerd, daarna LilyGO's specifieke [LilyGo-Cam-ESP32S3](https://github.com/Xinyuan-LilyGO/LilyGo-Cam-ESP32S3/tree/af0b94d6e79280bb6e8938ce3383ac19687957ca), met name `MinimalCameraExample.ino`, `utilities.h`, `app_httpd.cpp` en de README. De algemene Camera-Series-repository bevatte deze variant niet. Het project gebruikt de LilyGO-voedingsinitialisatie en het Espressif CameraWebServer-patroon voor MJPEG; gezichtsherkenning en uitgebreide camera-controls zijn weggelaten. Zie [bronvermelding](THIRD_PARTY_NOTICES.md).

De AXP2101 schakelt vóór camera-init ALDO1 naar 1,8 V, ALDO2 naar 2,8 V en ALDO4 naar 3,0 V. Zonder deze stap werkt alleen de pinmapping niet. Laadinstellingen blijven ongemoeid; acculaden en batterijbeheer vallen buiten deze versie.

## Bouwen en uploaden

Installeer VS Code met PlatformIO IDE, open deze map en gebruik **Build**, **Upload** en **Monitor**. Of voer in een PlatformIO-terminal uit:

```sh
pio run
pio device list
pio run -t upload --upload-port COM5
pio device monitor --port COM5 --baud 115200
```

Vervang `COM5` door de boardpoort (Linux bijvoorbeeld `/dev/ttyACM0`). Sluit de monitor vóór uploaden. Eerste build downloadt toolchain en libraries; alleen daarvoor is internet nodig. Camera- en HTTP-drivers komen uit het Arduino-framework; XPowersLib (v0.3.3) en U8g2 (tag 2.37.1, librarymetadata 2.36.19) zijn vastgezet op officiële releasecommits. Installeer ook Git zodat PlatformIO deze libraries kan ophalen.

Wordt het board niet gevonden, houd **BOOT** ingedrukt, druk kort **RESET**, laat BOOT los en upload opnieuw. Druk zo nodig na uploaden op RESET en selecteer de nieuwe USB-poort. De firmware wacht nooit op een Serial-monitor en start ook zelfstandig zonder computer.

## Verbinden

1. Zet het board aan en verbind de telefoon met wifi **HinnikCam**.
2. Standaard is dit een **open netwerk zonder wachtwoord**. Stel desgewenst `AP_PASSWORD` in `include/config.h` in op minimaal 8 tekens en flash opnieuw.
3. Kies op de telefoon **verbonden blijven zonder internet**. Schakel automatisch overschakelen naar mobiele data uit als de telefoon de lokale verbinding verlaat.
4. Open expliciet [http://192.168.4.1/](http://192.168.4.1/).

Directe MJPEG-stream: [http://192.168.4.1:81/stream](http://192.168.4.1:81/stream). Het korte adres [http://192.168.4.1/stream](http://192.168.4.1/stream) verwijst daarnaar door. Geen captive portal of HTTPS.

De pagina toont wifi-/camerastatus, IP-adres en het aantal wifi-clients. Status wordt apart op poort 80 opgehaald terwijl poort 81 streamt. 'Camera verstuurt beelden' betekent dat de server recent een frame heeft verzonden, niet dat de browser dat frame aantoonbaar heeft weergegeven.

Na verbreken ruimt de server de streamverbinding op en accepteert hij opnieuw een client. Verbind opnieuw met wifi en herlaad de pagina of druk **Stream opnieuw starten**. Automatisch herstellen van de browserstream is een vervolgstap.

## OLED-status

Het ingebouwde SSD1306-scherm (128x64, I2C-adres 0x3C) toont:

- HinnikCam
- Wifi: AP actief of FOUT
- IP-adres van het access point
- Aantal verbonden wifi-clients
- Camera: VGA JPEG OK of FOUT (initialisatiestatus)
- Stream: LIVE bij recent verzonden frames; anders geen beeld/niet gereed

De status ververst elke seconde. LIVE betekent dat de server binnen de laatste drie seconden een frame heeft verzonden; het bevestigt niet dat de telefoon dat frame heeft weergegeven. Een wifi-client hoeft geen stream te bekijken.

Het display gebruikt U8g2 met dezelfde SSD1306-configuratie en 180 graden rotatie als LilyGO's MinimalScreenExample. SDA 7 en SCL 6 worden gedeeld met de PMU; de camerabus blijft apart. Een ontbrekend OLED blokkeert de camera niet. Bij een latere I2C-fout stoppen OLED-updates tot een herstart.

In include/config.h staan OLED_ENABLED, OLED_ADDRESS, OLED_ROTATE_180, OLED_CONTRAST en OLED_REFRESH_MS. Zet OLED_ROTATE_180 op false als de tekst ondersteboven staat.

Controleer op hardware dat het aantal clients verandert bij verbinden/verbreken en dat Stream omschakelt tussen LIVE en geen beeld bij starten/stoppen van de browserstream. Controleer tegelijk dat het camerabeeld goed blijft doorlopen.

## Bestanden en configuratie

- `platformio.ini`: vastgezette buildomgeving, flash en PSRAM.
- `include/config.h`: wifi, camerabedrading, beeldoriëntatie, JPEG-kwaliteit en verzendlimiet.
- `src/main.cpp`: opstartvolgorde, clientlogging en OLED-updates.
- `src/display.cpp`, `include/display.h`: compact OLED-statusscherm.
- `src/camera.cpp`: AXP2101 en OV2640.
- `src/network.cpp`: zelfstandig access point, DHCP en vast IP.
- `src/webserver.cpp`, `include/web_page.h`: HTTP, status en MJPEG.
- `include/camera.h`, `include/network.h`, `include/webserver.h`: kleine module-interfaces.

Gecontroleerde camerapinnen: D0–D7 = **14, 47, 48, 21, 13, 11, 10, 9**; XCLK = **38**, PCLK = **12**, VSYNC = **8**, HREF = **18**, SCCB SDA/SCL = **5/4**, RESET = **39**, PWDN = **-1**. PMU SDA/SCL = **7/6**. PIR GPIO **17** is alleen gereserveerd. Verticaal spiegelen en horizontaal spiegelen volgen LilyGO's voorbeeld; pas `VFLIP`/`HMIRROR` aan bij afwijkende montage.

## Controle op echte hardware

De lokale build controleert compilatie; live beeld, voeding, PSRAM en bereik moeten op het fysieke board worden bevestigd:

1. Controleer Serial op circa 8 MB PSRAM, `OV2640 ready`, het AP-adres en beide HTTP-poorten.
2. Controleer bewegend VGA-beeld en bereikbaarheid van `/status` tijdens het streamen.
3. Laat ten minste 15 minuten streamen. Controleer vertraging en stabiliteit.
4. Zet telefoonwifi uit, wacht 10 seconden, verbind opnieuw en herlaad de pagina. Herhaal meerdere keren, zonder boardreset.
5. Test een koude start zonder aangesloten Serial-monitor en vervolgens de verbinding op de bedoelde afstand in de trailer.

## Bekende beperkingen

- Eén gelijktijdige stream; maximaal twee wifi-clients. Een tweede stream wacht zolang de eerste actief is.
- Geen gegarandeerde 10–15 fps of radiobereik; metaal in auto/trailer, afstand en weinig licht beïnvloeden het beeld. Geen nachtverlichting of audio.
- Bij netwerkverlies kan opruimen enkele seconden duren (HTTP-sendtimeout 3 seconden). Browserstream kan handmatig herstart nodig hebben; een browser kan het laatste beeld vasthouden.
- Bij camera-initfouten blijft de webpagina bereikbaar; controleer Serial, voeding en aansluiting en herstart daarna het board.
- Geen opname, PIR-verwerking, accubewaking of laadconfiguratie. Begin de hardwaretest via USB.
- Open AP: iedereen binnen bereik kan verbinden en kijken. Een configureerbaar WPA2-wachtwoord is beschikbaar.

## Mogelijke vervolgstappen

- PIR-trigger
- Battery voltage monitoring
- Uitgebreidere displaystatus (bijvoorbeeld batterijspanning)
- Fullscreen mobiele interface
- Automatisch reconnecten van browserstream
- Instelbare resolutie/framerate
- Low-battery waarschuwing
