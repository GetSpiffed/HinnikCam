#include <Arduino.h>
#include <WiFi.h>
#include <esp_camera.h>
#include "camera.h"
#include "network.h"
#include "webserver.h"
#include "display.h"
#include "power.h"

namespace {
bool cameraReady = false;
bool apReady = false;
bool webReady = false;
}

void setup() {
    Serial.begin(115200);
    // Standalone startup must never wait for USB Serial.
    delay(300);
    Serial.println("\nHinnikCam starting");
    const bool cameraPowered = startPower();
    cameraReady = cameraPowered && startCamera();
    startDisplay();
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
    updatePower(); // All runtime PMU and OLED I2C calls stay on this task.
    static int lastClients = -1;
    const int clients = WiFi.softAPgetStationNum();
    if (clients != lastClients) {
        Serial.printf("[wifi] Connected clients: %d\n", clients);
        lastClients = clients;
    }
    const wifi_mode_t mode = WiFi.getMode();
    updateDisplay(cameraReady, mode == WIFI_AP || mode == WIFI_AP_STA, webReady);
    if (powerOffDue()) {
        stopWebServer(); // Join stream task before releasing camera buffers.
        if (cameraReady) esp_camera_deinit();
        WiFi.mode(WIFI_OFF);
        sleepDisplay();
        powerOff();
    }
    delay(50);
}
