#include "network.h"
#include "config.h"
#include <WiFi.h>
#include <DNSServer.h>
#include <esp_netif.h>
#include <dhcpserver/dhcpserver.h>

namespace {
DNSServer dnsServer;
bool dnsRunning = false;

bool advertiseDns(const IPAddress &ip) {
    esp_netif_t *ap = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
    if (!ap) {
        Serial.println("[dns] ERROR: AP network interface missing");
        return false;
    }
    esp_err_t err = esp_netif_dhcps_stop(ap);
    if (err != ESP_OK && err != ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED) {
        Serial.printf("[dns] ERROR stopping DHCP: %s\n", esp_err_to_name(err));
        return false;
    }
    esp_netif_dns_info_t dns = {};
    dns.ip.type = ESP_IPADDR_TYPE_V4;
    dns.ip.u_addr.ip4.addr = static_cast<uint32_t>(ip);
    err = esp_netif_set_dns_info(ap, ESP_NETIF_DNS_MAIN, &dns);
    if (err == ESP_OK) {
        dhcps_offer_t offer = OFFER_DNS;
        err = esp_netif_dhcps_option(ap, ESP_NETIF_OP_SET,
            ESP_NETIF_DOMAIN_NAME_SERVER, &offer, sizeof(offer));
    }
    // Always restart DHCP, also if setting its DNS option failed.
    const esp_err_t restart = esp_netif_dhcps_start(ap);
    if (err != ESP_OK || restart != ESP_OK) {
        Serial.printf("[dns] ERROR DHCP DNS setup: %s; restart: %s\n",
            esp_err_to_name(err), esp_err_to_name(restart));
        return false;
    }
    return true;
}
}

bool startAccessPoint() {
    const IPAddress ip(Config::AP_IP[0], Config::AP_IP[1], Config::AP_IP[2], Config::AP_IP[3]);
    if (!WiFi.mode(WIFI_AP) || !WiFi.softAPConfig(ip, ip, IPAddress(255, 255, 255, 0)) ||
        !WiFi.softAP(Config::AP_SSID, Config::AP_PASSWORD, Config::AP_CHANNEL,
                     false, Config::AP_MAX_CLIENTS)) {
        Serial.println("[wifi] ERROR: AP startup failed");
        return false;
    }
    dnsServer.setTTL(Config::DNS_TTL_SECONDS);
    // Resolve our local name only; do not impersonate internet connectivity probes.
    // Arduino DNSServer also accepts the www. prefix and case-insensitive names.
    dnsRunning = dnsServer.start(53, Config::LOCAL_HOSTNAME, ip);
    if (!dnsRunning || !advertiseDns(ip)) {
        Serial.println("[dns] ERROR: local DNS startup failed");
        stopLocalDns();
        return false;
    }
    Serial.printf("[wifi] AP %s: %s\n", Config::AP_SSID, ip.toString().c_str());
    Serial.printf("[dns] http://%s -> %s; DHCP DNS active, TTL %lu s\n",
        Config::LOCAL_HOSTNAME, ip.toString().c_str(),
        static_cast<unsigned long>(Config::DNS_TTL_SECONDS));
    return true;
}

void processLocalDns() {
    if (dnsRunning) dnsServer.processNextRequest();
}

void stopLocalDns() {
    if (dnsRunning) dnsServer.stop();
    dnsRunning = false;
}