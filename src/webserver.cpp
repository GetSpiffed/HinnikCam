// MJPEG chunking / two-server pattern adapted from Espressif CameraWebServer
// as distributed in LilyGO MinimalCameraExample/app_httpd.cpp.
// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
// SPDX-License-Identifier: Apache-2.0
#include "webserver.h"
#include "config.h"
#include "web_page.h"
#include "power.h"
#include <WiFi.h>
#include <atomic>
#include <esp_camera.h>
#include <esp_http_server.h>

namespace {
httpd_handle_t webServer = nullptr, streamServer = nullptr;
bool cameraReady = false; // Written before either HTTP task starts.
std::atomic<bool> streaming{false};
std::atomic<uint32_t> lastFrameMs{0};
constexpr char BOUNDARY[] = "\r\n--hinnikcamframe\r\n";

esp_err_t indexHandler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html; charset=utf-8");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    return httpd_resp_send(req, WEB_PAGE, HTTPD_RESP_USE_STRLEN);
}

esp_err_t statusHandler(httpd_req_t *req) {
    char json[400];
    const PowerStatus power = getPowerStatus();
    snprintf(json, sizeof(json),
        R"JSON({"camera_ready":%s,"streaming":%s,"frame_age_ms":%lu,"clients":%u,"ip":"%s","hostname":"%s","power_ready":%s,"usb":%s,"battery":%s,"charging":%s,"battery_mv":%u,"shutting_down":%s})JSON",
        cameraReady ? "true" : "false", streaming.load() ? "true" : "false",
        static_cast<unsigned long>(millis() - lastFrameMs.load()),
        WiFi.softAPgetStationNum(), WiFi.softAPIP().toString().c_str(), Config::LOCAL_HOSTNAME,
        power.ready ? "true" : "false", power.usb ? "true" : "false",
        power.battery ? "true" : "false", power.charging ? "true" : "false",
        power.batteryMv, powerOffPending() ? "true" : "false");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    return httpd_resp_send(req, json, HTTPD_RESP_USE_STRLEN);
}

esp_err_t captureHandler(httpd_req_t *req) {
    if (!cameraReady || powerOffPending()) {
        httpd_resp_set_status(req, "503 Service Unavailable");
        return httpd_resp_send(req, "Camera unavailable", HTTPD_RESP_USE_STRLEN);
    }
    // The driver owns a queue of two PSRAM buffers. A still capture borrows
    // one independently of the stream; never reconfigure or restart the sensor.
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("[camera] ERROR: photo capture failed");
        return httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Capture failed");
    }
    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    httpd_resp_set_hdr(req, "Content-Disposition", "attachment; filename=HinnikCam.jpg");
    const esp_err_t result = httpd_resp_send(req, reinterpret_cast<const char *>(fb->buf), fb->len);
    esp_camera_fb_return(fb); // Also release on browser timeout/disconnect.
    Serial.printf("[capture] Photo request: %s\n", esp_err_to_name(result));
    return result;
}

esp_err_t shutdownHandler(httpd_req_t *req) {
    // Custom header prevents accidental GETs and simple cross-origin form POSTs.
    char confirm[8] = {};
    if (httpd_req_get_hdr_value_str(req, "X-HinnikCam-Confirm", confirm, sizeof(confirm)) != ESP_OK ||
        strcmp(confirm, "yes") != 0) {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Confirmation required");
    }
    if (!requestPowerOff()) {
        httpd_resp_set_status(req, "503 Service Unavailable");
        return httpd_resp_send(req, "Power controller unavailable", HTTPD_RESP_USE_STRLEN);
    }
    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    return httpd_resp_send(req, "{\"shutting_down\":true}", HTTPD_RESP_USE_STRLEN);
}

esp_err_t redirectHandler(httpd_req_t *req) {
    char url[64];
    snprintf(url, sizeof(url), "http://%s:%u/stream", WiFi.softAPIP().toString().c_str(), Config::STREAM_PORT);
    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", url);
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    return httpd_resp_send(req, nullptr, 0);
}

esp_err_t streamHandler(httpd_req_t *req) {
    if (!cameraReady) {
        httpd_resp_set_status(req, "503 Service Unavailable");
        return httpd_resp_send(req, "Camera initialization failed; check Serial", HTTPD_RESP_USE_STRLEN);
    }
    httpd_resp_set_type(req, "multipart/x-mixed-replace;boundary=hinnikcamframe");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    Serial.println("[stream] Client connected");
    lastFrameMs.store(millis() - 3000); // No fresh frame until the first successful send.
    streaming.store(true);
    esp_err_t result = ESP_OK;
    while (result == ESP_OK && !powerOffPending()) {
        const uint32_t started = millis();
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("[camera] ERROR: frame capture failed");
            result = ESP_FAIL;
            break;
        }
        char header[80];
        const int size = snprintf(header, sizeof(header),
            "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", static_cast<unsigned>(fb->len));
        result = httpd_resp_send_chunk(req, BOUNDARY, sizeof(BOUNDARY) - 1);
        if (result == ESP_OK) result = httpd_resp_send_chunk(req, header, size);
        if (result == ESP_OK) result = httpd_resp_send_chunk(req, reinterpret_cast<const char *>(fb->buf), fb->len);
        // Return the buffer on every path, including a disconnected client.
        esp_camera_fb_return(fb);
        if (result == ESP_OK) {
            lastFrameMs.store(millis());
            const uint32_t elapsed = millis() - started;
            const uint32_t interval = 1000 / Config::TARGET_FPS;
            delay(elapsed < interval ? interval - elapsed : 1);
        }
    }
    streaming.store(false);
    Serial.printf("[stream] Connection ended (%s); ready for reconnect\n", esp_err_to_name(result));
    // An error return closes the dead socket; the server continues accepting clients.
    return result == ESP_OK ? ESP_FAIL : result; // Close the multipart socket on shutdown too.
}

bool addHandler(httpd_handle_t server, const char *uri, esp_err_t (*handler)(httpd_req_t *), httpd_method_t method = HTTP_GET) {
    httpd_uri_t route = {};
    route.uri = uri; route.method = method; route.handler = handler;
    const esp_err_t err = httpd_register_uri_handler(server, &route);
    if (err != ESP_OK) Serial.printf("[web] ERROR route %s: %s\n", uri, esp_err_to_name(err));
    return err == ESP_OK;
}
}

bool startWebServer(bool ready) {
    cameraReady = ready;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = Config::WEB_PORT;
    config.lru_purge_enable = true;
    config.send_wait_timeout = 3;
    config.recv_wait_timeout = 3;
    esp_err_t err = httpd_start(&webServer, &config);
    if (err != ESP_OK) {
        Serial.printf("[web] ERROR startup: %s\n", esp_err_to_name(err));
        return false;
    }
    if (!addHandler(webServer, "/", indexHandler) || !addHandler(webServer, "/status", statusHandler) ||
        !addHandler(webServer, "/stream", redirectHandler) ||
        !addHandler(webServer, "/shutdown", shutdownHandler, HTTP_POST) ||
        !addHandler(webServer, "/capture", captureHandler)) return false;
    // A synchronous MJPEG handler occupies its HTTP task. Keep UI/status separate.
    config.server_port = Config::STREAM_PORT;
    config.ctrl_port += 1;
    err = httpd_start(&streamServer, &config);
    if (err != ESP_OK) {
        Serial.printf("[web] ERROR stream startup: %s\n", esp_err_to_name(err));
        return false;
    }
    if (!addHandler(streamServer, "/stream", streamHandler)) return false;
    Serial.printf("[web] UI port %u; MJPEG port %u\n", Config::WEB_PORT, Config::STREAM_PORT);
    return true;
}

bool streamHasRecentFrames() {
    return streaming.load() && millis() - lastFrameMs.load() < 3000;
}
void stopWebServer() {
    // powerOffPending() tells the synchronous stream handler to finish.
    if (streamServer) { httpd_stop(streamServer); streamServer = nullptr; }
    if (webServer) { httpd_stop(webServer); webServer = nullptr; }
}