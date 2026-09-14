#include "network.h"
#include "config.h"
#include <WiFi.h>

bool startAccessPoint() {
    const IPAddress ip(Config::AP_IP[0], Config::AP_IP[1], Config::AP_IP[2], Config::AP_IP[3]);
    if (!WiFi.mode(WIFI_AP) || !WiFi.softAPConfig(ip, ip, IPAddress(255, 255, 255, 0)) ||
        !WiFi.softAP(Config::AP_SSID, Config::AP_PASSWORD, Config::AP_CHANNEL,
                     false, Config::AP_MAX_CLIENTS)) {
        Serial.println("[wifi] ERROR: AP startup failed");
        return false;
    }
    Serial.printf("[wifi] AP %s: %s\n", Config::AP_SSID, WiFi.softAPIP().toString().c_str());
    return true;
}
