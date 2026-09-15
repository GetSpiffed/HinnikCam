// AXP2101 setup follows LilyGO MinimalPowersExample (MIT).
// Copyright (c) 2022 Shenzhen Xin Yuan Electronic Technology Co., Ltd
#include "power.h"
#include "config.h"
#include "display.h"
#include <Wire.h>
#include <atomic>
#define XPOWERS_CHIP_AXP2101
#include <XPowersLib.h>

namespace {
XPowersPMU pmu;
bool ready = false;
bool pirReady = false;
bool lastPir = false;
uint32_t lastSample = 0;
uint32_t requestedAt = 0;
bool countdown = false;
std::atomic<bool> pending{false};
// Single atomic snapshot; HTTP tasks never touch the shared PMU/OLED I2C bus.
std::atomic<uint32_t> telemetry{0};

void samplePower() {
    const bool battery = pmu.isBatteryConnect();
    const uint16_t mv = battery ? pmu.getBattVoltage() : 0;
    telemetry.store((1UL << 16) | (pmu.isVbusIn() ? 1UL << 17 : 0) |
        (battery ? 1UL << 18 : 0) | (pmu.isCharging() ? 1UL << 19 : 0) | mv);
}
}

bool startPower() {
    if (!pmu.begin(Wire, AXP2101_SLAVE_ADDRESS, Config::PMU_SDA, Config::PMU_SCL)) {
        Serial.println("[power] ERROR: AXP2101 not found");
        return false;
    }
    ready = true;
    // Camera voltage rails are specified by LilyGO; do not change voltages.
    const bool cameraPower = pmu.setALDO1Voltage(1800) && pmu.enableALDO1() &&
        pmu.setALDO2Voltage(2800) && pmu.enableALDO2() &&
        pmu.setALDO4Voltage(3000) && pmu.enableALDO4();
    if (!cameraPower) Serial.println("[power] ERROR: camera power rails failed");
    pirReady = pmu.setALDO3Voltage(3300) && pmu.enableALDO3();
    pinMode(Config::PIR_INPUT, INPUT);
    if (!pirReady) Serial.println("[power] ERROR: PIR power unavailable");
    // Hardware long-press shutdown remains available even if firmware hangs.
    if (!pmu.setPowerKeyPressOnTime(XPOWERS_POWERON_128MS) ||
        !pmu.setPowerKeyPressOffTime(XPOWERS_POWEROFF_6S)) {
        Serial.println("[power] ERROR: power key timing setup failed");
    }
    pmu.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
    pmu.clearIrqStatus();
    pmu.enableIRQ(XPOWERS_AXP2101_PKEY_SHORT_IRQ);
    pinMode(Config::PMU_IRQ, INPUT_PULLUP);
    pmu.enableBattDetection();
    pmu.enableBattVoltageMeasure();
    // Charging configuration is intentionally left unchanged.
    delay(100);
    samplePower();
    Serial.println("[power] Ready: PWRKEY short = OLED, hold 6 s = off");
    return cameraPower;
}

PowerStatus getPowerStatus() {
    const uint32_t value = telemetry.load();
    return {bool(value & (1UL << 16)), bool(value & (1UL << 17)),
        bool(value & (1UL << 18)), bool(value & (1UL << 19)),
        static_cast<uint16_t>(value & 0xffff)};
}

void updatePower() {
    if (!ready) return;
    if (digitalRead(Config::PMU_IRQ) == LOW) {
        pmu.getIrqStatus();
        const bool pressed = pmu.isPekeyShortPressIrq();
        pmu.clearIrqStatus();
        if (pressed) {
            Serial.println("[power] PWRKEY: wake OLED");
            wakeDisplay();
        }
    }
    const bool pir = pirReady && digitalRead(Config::PIR_INPUT) == HIGH;
    if (pir) wakeDisplay(); // Sustained activity keeps the screen awake.
    if (pir && !lastPir) Serial.println("[pir] Activity: wake OLED");
    lastPir = pir;
    const uint32_t now = millis();
    if (now - lastSample >= 2000) {
        lastSample = now;
        samplePower();
    }
    if (pending.load() && !countdown) {
        requestedAt = now;
        countdown = true;
        wakeDisplay();
        Serial.println("[power] Browser shutdown requested");
    }
}

bool requestPowerOff() {
    if (!getPowerStatus().ready) return false;
    pending.store(true);
    return true;
}
bool powerOffPending() { return pending.load(); }
bool powerOffDue() {
    return countdown && millis() - requestedAt >= 2000;
}

void powerOff() {
    Serial.println("[power] Switching off via AXP2101");
    pmu.shutdown();
    // If shutdown failed or external power immediately restarted the PMU,
    // do not leave a running device permanently without its camera/network.
    delay(2000);
    Serial.println("[power] ERROR: still running after shutdown; rebooting");
    ESP.restart();
}
