// SSD1306 setup follows LilyGO MinimalScreenExample (MIT).
// Copyright (c) 2022 Shenzhen Xin Yuan Electronic Technology Co., Ltd
#include "display.h"
#include "config.h"
#include "webserver.h"
#include "power.h"
#include <U8g2lib.h>
#include <WiFi.h>
#include <Wire.h>

namespace {
U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(
    Config::OLED_ROTATE_180 ? U8G2_R2 : U8G2_R0,
    U8X8_PIN_NONE); // Wire already owns pins 7/6; do not reset their GPIO mode.
bool available = false;
uint32_t lastUpdate = 0;
bool firstUpdate = true;
bool sleeping = false;
uint32_t lastActivity = 0;
}

bool startDisplay() {
    if (!Config::OLED_ENABLED) return false;
    // Reuse PMU Wire bus; camera SCCB uses different pins and controller.
    Serial.println("[oled] Starting I2C");
    if (!Wire.begin(Config::PMU_SDA, Config::PMU_SCL)) {
        Serial.println("[oled] I2C unavailable; continuing without display");
        return false;
    }
    Serial.println("[oled] Probing SSD1306");
    Wire.setTimeOut(50);
    Wire.beginTransmission(Config::OLED_ADDRESS);
    if (Wire.endTransmission() != 0) {
        Serial.println("[oled] SSD1306 not found; continuing without display");
        return false;
    }
    Serial.println("[oled] Initializing SSD1306");
    oled.setI2CAddress(Config::OLED_ADDRESS << 1); // U8g2 uses 8-bit addresses.
    oled.setBusClock(100000);
    oled.begin();
    Serial.println("[oled] Setting contrast");
    oled.setContrast(Config::OLED_CONTRAST);
    oled.setFont(u8g2_font_6x10_tf);
    available = true;
    lastActivity = millis();
    Serial.println("[oled] SSD1306 ready: 128x64, SDA 7 / SCL 6");
    return true;
}

void updateDisplay(bool cameraReady, bool apReady, bool webReady) {
    if (!available) return;
    const uint32_t now = millis();
    if (!sleeping && now - lastActivity >= Config::OLED_IDLE_MS) sleepDisplay();
    if (sleeping) return;
    if (!firstUpdate && now - lastUpdate < Config::OLED_REFRESH_MS) return;
    firstUpdate = false;
    lastUpdate = now;
    // Stop trying if the shared I2C bus/display becomes unavailable.
    Wire.beginTransmission(Config::OLED_ADDRESS);
    if (Wire.endTransmission() != 0) {
        available = false;
        Serial.println("[oled] Display disconnected; updates disabled until reboot");
        return;
    }
    char line[24];
    oled.clearBuffer();
    oled.drawStr(0, 10, powerOffPending() ? "Uitschakelen..." : "HinnikCam");
    oled.drawHLine(0, 13, 128);
    oled.drawStr(0, 23, apReady ? "Wifi: AP actief" : "Wifi: FOUT");
    snprintf(line, sizeof(line), "http://%s", Config::LOCAL_HOSTNAME);
    oled.drawStr(0, 33, line);
    snprintf(line, sizeof(line), "Wifi-clients: %u", WiFi.softAPgetStationNum());
    oled.drawStr(0, 43, line);
    oled.drawStr(0, 53, !webReady ? "Webserver: FOUT" :
        !cameraReady ? "Camera: FOUT" :
        streamHasRecentFrames() ? "Camera: LIVE" : "Camera: gereed");
    const PowerStatus power = getPowerStatus();
    if (!power.ready) snprintf(line, sizeof(line), "Voeding: onbekend");
    else if (power.battery && power.batteryMv > 0)
        snprintf(line, sizeof(line), "%s %u.%02uV%s",
            power.usb ? "USB+accu" : "Accu",
            power.batteryMv / 1000, (power.batteryMv % 1000) / 10,
            power.charging ? " laden" : "");
    else snprintf(line, sizeof(line), "%s",
        power.usb ? "USB | geen accumeting" : "Geen accumeting");
    oled.drawStr(0, 63, line);
    oled.sendBuffer();
}
void wakeDisplay() {
    if (!available) return;
    lastActivity = millis();
    if (sleeping) {
        oled.setPowerSave(0);
        sleeping = false;
        firstUpdate = true;
        Serial.println("[oled] Awake");
    }
}

void sleepDisplay() {
    if (!available || sleeping) return;
    oled.setPowerSave(1);
    sleeping = true;
    Serial.println("[oled] Sleeping; camera remains active");
}