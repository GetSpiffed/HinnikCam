// Power setup and board mapping adapted from LilyGO MinimalCameraExample.
// Copyright (c) 2022 Shenzhen Xin Yuan Electronic Technology Co., Ltd (MIT).
#include "camera.h"
#include "config.h"
#include <esp_camera.h>

bool startCamera() {
    if (!psramFound()) {
        Serial.println("[camera] ERROR: PSRAM missing; check qio_opi configuration");
        return false;
    }
    Serial.printf("[camera] PSRAM: %u bytes\n", ESP.getPsramSize());
    camera_config_t c = {};
    c.ledc_channel = LEDC_CHANNEL_0;
    c.ledc_timer = LEDC_TIMER_0;
    c.pin_d0 = Config::CAM_D0; c.pin_d1 = Config::CAM_D1;
    c.pin_d2 = Config::CAM_D2; c.pin_d3 = Config::CAM_D3;
    c.pin_d4 = Config::CAM_D4; c.pin_d5 = Config::CAM_D5;
    c.pin_d6 = Config::CAM_D6; c.pin_d7 = Config::CAM_D7;
    c.pin_xclk = Config::CAM_XCLK; c.pin_pclk = Config::CAM_PCLK;
    c.pin_vsync = Config::CAM_VSYNC; c.pin_href = Config::CAM_HREF;
    c.pin_sccb_sda = Config::CAM_SDA; c.pin_sccb_scl = Config::CAM_SCL;
    c.pin_pwdn = Config::CAM_PWDN; c.pin_reset = Config::CAM_RESET;
    c.xclk_freq_hz = 20000000;
    c.pixel_format = PIXFORMAT_JPEG;
    c.frame_size = FRAMESIZE_VGA;
    c.jpeg_quality = Config::JPEG_QUALITY;
    c.fb_count = 2;
    c.fb_location = CAMERA_FB_IN_PSRAM;
    c.grab_mode = CAMERA_GRAB_LATEST;
    const esp_err_t err = esp_camera_init(&c);
    if (err != ESP_OK) {
        Serial.printf("[camera] ERROR init: %s (0x%x)\n", esp_err_to_name(err), err);
        return false;
    }
    sensor_t *sensor = esp_camera_sensor_get();
    if (!sensor || sensor->id.PID != OV2640_PID ||
        sensor->set_vflip(sensor, Config::VFLIP) != 0 ||
        sensor->set_hmirror(sensor, Config::HMIRROR) != 0) {
        Serial.println("[camera] ERROR: expected OV2640 or orientation setup failed");
        esp_camera_deinit();
        return false;
    }
    Serial.println("[camera] OV2640 ready: VGA JPEG, two PSRAM frame buffers");
    return true;
}
