#pragma once
#include <Arduino.h>

namespace Config {
constexpr char AP_SSID[] = "HinnikCam";
constexpr char AP_PASSWORD[] = ""; // Open AP; set >= 8 characters for WPA2.
constexpr uint8_t AP_IP[] = {192, 168, 4, 1};
constexpr uint8_t AP_CHANNEL = 6;
constexpr uint8_t AP_MAX_CLIENTS = 2; // One simultaneous stream supported.
constexpr uint16_t WEB_PORT = 80;
constexpr uint16_t STREAM_PORT = 81;
constexpr uint8_t TARGET_FPS = 12; // Send-rate ceiling, not a sensor FPS guarantee.
constexpr int JPEG_QUALITY = 12; // Lower is better quality / more traffic.
constexpr bool VFLIP = true;
constexpr bool HMIRROR = true; // Orientation from LilyGO MinimalCameraExample.
static_assert(TARGET_FPS > 0 && TARGET_FPS <= 30, "Invalid frame rate");

// LilyGo-Cam-ESP32S3: LILYGO_ESP32S3_CAM_PIR_VOICE, NOT Plus/SIM.
constexpr int CAM_D0 = 14, CAM_D1 = 47, CAM_D2 = 48, CAM_D3 = 21;
constexpr int CAM_D4 = 13, CAM_D5 = 11, CAM_D6 = 10, CAM_D7 = 9;
constexpr int CAM_XCLK = 38, CAM_PCLK = 12, CAM_VSYNC = 8, CAM_HREF = 18;
constexpr int CAM_SDA = 5, CAM_SCL = 4, CAM_RESET = 39, CAM_PWDN = -1;
constexpr int PMU_SDA = 7, PMU_SCL = 6;
constexpr bool OLED_ENABLED = true;
constexpr uint8_t OLED_ADDRESS = 0x3C;
constexpr bool OLED_ROTATE_180 = true; // U8G2_R2, matching LilyGO.
constexpr uint8_t OLED_CONTRAST = 128;
constexpr uint32_t OLED_REFRESH_MS = 1000;
constexpr int PIR_INPUT = 17; // Reserved only: no polling or interrupts in v1.
}
