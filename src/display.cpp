// SSD1306 setup follows LilyGO MinimalScreenExample (MIT).
// Copyright (c) 2022 Shenzhen Xin Yuan Electronic Technology Co., Ltd
#include "display.h"
#include "config.h"
#include "webserver.h"
#include "power.h"
#include <U8g2lib.h>
#include <WiFi.h>
#include <Wire.h>
#include <cmath>

namespace {
U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(
    Config::OLED_ROTATE_180 ? U8G2_R2 : U8G2_R0,
    U8X8_PIN_NONE); // Wire already owns pins 7/6; do not reset their GPIO mode.
bool available = false;
uint32_t lastUpdate = 0;
bool firstUpdate = true;
bool sleeping = false;
uint32_t lastActivity = 0;
enum class Animation { None, Welcome, Goodbye };
Animation animation = Animation::None;
uint32_t animationStarted = 0;
uint32_t lastFrame = 0;

// Original tiny pixel horse, facing right. Legs are posed separately below.
const char* const horse[] = {
    "                  # #",
    "                 #####",
    "                #######",
    "                ## #####",
    "               ###  ###",
    "              ####",
    "   ################",
    " #################",
    "## ###############",
    "#   #############",
    "#    ###########"
};

float ease(float t) {
    t = fmaxf(0, fminf(1, t));
    return t * t * (3 - 2 * t);
}

void drawHorse(float x, float rear, float phase, float run, float crouch) {
    const float angle = -0.72f * rear + 0.035f * sinf(phase) * run;
    const float c = cosf(angle), s = sinf(angle);
    // Rotate around the haunches, not the feet: keep the hips above their support.
    const float shiftX = -12*s + rear;
    const float bounce = -2.5f * fmaxf(0, sinf(phase)) * run +
        12*(c-1) + 2*crouch - rear;
    auto pixel = [&](float px, float py) {
        const float dx = (px - 7) * 1.5f, dy = (py - 18) * 1.5f;
        const int sx = lroundf(x + shiftX + dx * c - dy * s);
        const int sy = lroundf(49 + bounce + dx * s + dy * c);
        if (sx >= 0 && sx < 127 && sy >= 14 && sy < 51)
            oled.drawBox(sx, sy, 2, 2);
    };
    auto limb = [&](float ax, float ay, float bx, float by) {
        const int steps = static_cast<int>(ceilf(fmaxf(fabsf(bx-ax), fabsf(by-ay)) * 2));
        for (int i = 0; i <= steps; ++i) {
            const float t = steps ? float(i) / steps : 0;
            pixel(ax + (bx-ax)*t, ay + (by-ay)*t);
        }
    };
    // Four individually phased, jointed legs: reach, push, tuck, extend.
    for (int leg = 0; leg < 4; ++leg) {
        const bool front = leg >= 2;
        const float hip = front ? 15 + (leg-2)*2 : 7 + leg*2;
        const float p = phase + (front ? 2.2f : 0) + (leg % 2)*0.8f;
        const float swing = sinf(p) * run;
        const float tuck = fmaxf(0, cosf(p)) * run;
        float kneeX = hip + 2.8f*swing;
        float kneeY = 14 - 2*tuck;
        float hoofX = hip + 5*swing;
        float hoofY = 18 - 5*tuck;
        if (front) {
            // Paw lightly with the forelegs while balancing in the rear.
            const float paw = sinf(phase + (leg - 2) * 1.7f) * rear;
            kneeX += 3*rear + 1.1f*paw;
            kneeY -= 2*rear + 0.6f*fabsf(paw);
            hoofX = hoofX*(1-rear) + (hip+1 + 1.4f*paw)*rear;
            hoofY = hoofY*(1-rear) + (12 - 0.8f*paw)*rear;
        } else {
            // Slight flex only: keep the supporting legs almost straight.


            // Inverse body rotation keeps the supporting hooves on the ground.
            const float groundX = (hip-7)*1.5f - shiftX;
            const float plantedX = 7 + (groundX*c - bounce*s)/1.5f;
            const float plantedY = 18 + (-groundX*s - bounce*c)/1.5f;
            const float support = 1-run;
            hoofX = hoofX*(1-support) + plantedX*support;
            hoofY = hoofY*(1-support) + plantedY*support;
            // Position the joint near the hip-to-hoof line, avoiding a deep squat.
            kneeX = kneeX*(1-support) + ((hip+hoofX)*0.5f - 1.8f*crouch - 0.4f*rear)*support;
            kneeY = kneeY*(1-support) + ((10+hoofY)*0.5f)*support;
        }
        limb(hip, 10, kneeX, kneeY);
        limb(kneeX, kneeY, hoofX, hoofY);
        limb(hoofX, hoofY, hoofX+1, hoofY);
    }
    for (int y = 0; y < 11; ++y)
        for (int col = 3; horse[y][col]; ++col)
            if (horse[y][col] == '#') pixel(col, y);
    // Tail flicks behind the body, with a delayed tip.
    const float swish = sinf(phase-0.7f)*run + rear*(0.7f + 0.35f*sinf(phase));
    limb(4, 7, 1, 8 + swish);
    limb(1, 8 + swish, -2, 11 + 2*swish);
}

void drawAnimation(uint32_t elapsed) {
    const bool goodbye = animation == Animation::Goodbye;
    // 1.6 s for anticipation, rise, hold, landing; 1.6 s for the gallop.
    const float time = elapsed;
    const float rearTime = goodbye ? time : time - 1600;
    float rear = 0, crouch = 0;
    if (rearTime >= 0 && rearTime < 1600) {
        // A natural rear is: load the hindquarters, push upward, balance, land.
        if (rearTime < 320) {
            crouch = 1.10f*ease(rearTime/320);
        } else if (rearTime < 820) {
            rear = ease((rearTime-320)/500);
            crouch = 1.10f - 0.32f*rear;
        } else if (rearTime < 1120) {
            const float balance = (rearTime-820) / 300.0f * 6.2831853f;
            rear = 0.94f + 0.045f*sinf(balance);
            crouch = 0.78f + 0.07f*cosf(balance);
        } else if (rearTime < 1450) {
            rear = 0.94f*(1-ease((rearTime-1120)/330));
            crouch = 0.78f*(1-ease((rearTime-1120)/330));
        } else {
            crouch = 0.25f*sinf((rearTime-1450)/150*3.14159265f);
        }
    }
    float x = 53, run = 0, phase = 0;
    if (!goodbye && time < 1600) {
        const float t = time/1600;
        // Ease into the stopping position; settle the final stride.
        x = -35 + 88*(1-(1-t)*(1-t));
        run = 1-ease((t-0.75f)/0.25f);
        phase = time/440*6.2831853f;
    } else if (goodbye && time >= 1600) {
        const float t = (time-1600)/1600;
        x = 53 + 110*t*t;
        run = ease(t/0.15f);
        phase = (time-1600)/440*6.2831853f;
    }
    if (rearTime >= 0 && rearTime < 1600) {
        // Keep legs, tail and balance alive during the rear too.
        phase = rearTime / 260.0f * 6.2831853f;
    }
    oled.clearBuffer();
    oled.drawStr(37, 10, "SpecialCam");
    drawHorse(x, rear, phase, run, crouch);
    oled.drawHLine(0, 52, 128);
    const char* caption = goodbye ? "Tot de volgende rit!" : "Klaar voor de rit!";
    oled.drawStr((128 - oled.getStrWidth(caption)) / 2, 63, caption);
    // SSD1306 fast-mode transfer keeps frames fluid. Restore the shared PMU bus.
    oled.setBusClock(400000);
    oled.sendBuffer();
    oled.setBusClock(100000);
    Wire.setClock(100000);
}
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
    if (animation != Animation::None) {
        const uint32_t elapsed = now - animationStarted;
        if (elapsed < Config::OLED_ANIMATION_MS) {
            if (now - lastFrame >= 60) {
                lastFrame = now;
                drawAnimation(elapsed);
            }
            return;
        }
        if (animation == Animation::Goodbye) return; // Hold until power-off.
        animation = Animation::None;
        firstUpdate = true;
        lastActivity = now;
    }
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
    oled.drawStr(0, 10, powerOffPending() ? "Uitschakelen..." : "SpecialCam");
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
void startDisplayAnimation(bool shutdown) {
    if (!available) return;
    wakeDisplay();
    animation = shutdown ? Animation::Goodbye : Animation::Welcome;
    animationStarted = millis();
    lastFrame = animationStarted - 60;
    Serial.println(shutdown ? "[oled] Goodbye animation" : "[oled] Welcome animation");
}
