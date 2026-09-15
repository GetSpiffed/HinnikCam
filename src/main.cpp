#include <Arduino.h>
#include <WiFi.h>
#include "camera.h"
#include "network.h"
#include "webserver.h"
#include "display.h"

namespace {
bool cameraReady = false;
bool apReady = false;
bool webReady = false;
}

void setup() {
    Serial.begin(115200);
    // Never wait for USB Serial: startup must also work on standalone power.
    delay(300);
    Serial.println("\nHinnikCam starting");
    cameraReady = startCamera();
    startDisplay(); // Optional: failure must not prevent camera/network startup.
    // Keep diagnostics accessible even when camera initialization fails.
    apReady = startAccessPoint();
    webReady = apReady && startWebServer(cameraReady);
    updateDisplay(cameraReady, apReady, webReady);
    if (!apReady || !webReady) {
        Serial.println("[boot] ERROR: network/webserver failed; restarting in 5 seconds");
        delay(5000);
        ESP.restart();
    }
}

void loop() {
    static int lastClients = -1;
    const int clients = WiFi.softAPgetStationNum();
    if (clients != lastClients) {
        Serial.printf("[wifi] Connected clients: %d\n", clients);
        lastClients = clients;
    }
    const wifi_mode_t mode = WiFi.getMode();
    updateDisplay(cameraReady, mode == WIFI_AP || mode == WIFI_AP_STA, webReady);
    // Future PIR/battery work can be added here without owning the stream.
    delay(500);
}