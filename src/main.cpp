#include <Arduino.h>
#include <WiFi.h>
#include "camera.h"
#include "network.h"
#include "webserver.h"

void setup() {
    Serial.begin(115200);
    // Never wait for USB Serial: startup must also work on standalone power.
    delay(300);
    Serial.println("\nHinnikCam starting");
    const bool cameraReady = startCamera();
    // Keep diagnostics accessible even when camera initialization fails.
    if (!startAccessPoint() || !startWebServer(cameraReady)) {
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
    // Future PIR/battery/display work can be added here without owning the stream.
    delay(500);
}
