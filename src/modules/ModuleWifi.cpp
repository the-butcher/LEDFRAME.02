#include "ModuleWifi.h"

#include <ArduinoJson.h>

void ModuleWifi::powerup(wifi_mode_____e mode) {

    if (ModuleWifi::getClientState() != mode) {

        WiFi.persistent(true);
        esp_wifi_set_max_tx_power(40);

        // ARDUINO_EVENT_WIFI_STA_CONNECTED
        // WiFi.onEvent(ModuleWifi::handleStationDisconnected, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
        if (ModuleWifi::isConnected()) {  // disconnect if already active
            ModuleMqtt::depower();        // disconnect mqtt
            ModuleHttp::depower();        // stop http server
            ModuleWifi::depower();
        }

        if (mode == WIFI_MODE____STATION) {
            WiFi.mode(WIFI_STA);
            WiFi.begin(ModuleConfig::connConfig.wifi.ssid, ModuleConfig::connConfig.wifi.wpwd);
            for (int i = 0; i < 200; i++) {
                delay(10);
                if (WiFi.isConnected()) {
                    break;
                };
            }
        } else if (mode == WIFI_MODE_________AP) {
            WiFi.mode(WIFI_AP);
            WiFi.softAP(WIFI_NAME_AP.c_str(), NULL);
            for (int i = 0; i < 200; i++) {  // wait for connection
                delay(10);
                if (WiFi.isConnected()) {
                    break;
                };
            }
        }
    }
}

wifi_mode_____e ModuleWifi::getClientState() {
    wifi_mode_t wifiMode = WiFi.getMode();
    if (wifiMode == WIFI_AP) {
        return WIFI_MODE_________AP;
    } else if (WiFi.isConnected()) {
        return WIFI_MODE____STATION;
    } else {
        return WIFI_MODE________OFF;
    }
}

void ModuleWifi::handleStationDisconnected(WiFiEvent_t event) {
    // nothing
}

/**
 * can not go to sleep in AP mode, all other modes
 */
bool ModuleWifi::isReadyToSleep() {
    return ModuleWifi::getClientState() != WIFI_MODE_________AP;  // may not sleep in wifi mode AP
}

bool ModuleWifi::isConnected() {
    wifi_mode_t wifiMode = WiFi.getMode();
    return wifiMode == WIFI_AP || WiFi.isConnected();  // WiFi.isConnected() includes both WIFI_MODE_STA and WIFI_MODE_APSTA
}

void ModuleWifi::depower() {
    if (ModuleWifi::isConnected()) {
        WiFi.softAPdisconnect(true);
        WiFi.disconnect(true);
        for (int i = 0; i < 200; i++) {
            delay(10);
            if (!ModuleWifi::isConnected()) {
                break;
            }
        }
    }
    WiFi.mode(WIFI_OFF);
}

String ModuleWifi::getAddress() {
    wifi_mode_t wifiMode = WiFi.getMode();
    if (wifiMode == WIFI_STA) {
        return WiFi.localIP().toString();
    } else if (wifiMode == WIFI_AP) {
        return WiFi.softAPIP().toString();
    } else if (wifiMode == WIFI_OFF) {
        return "wifi off";
    } else {
        return String(wifiMode);
    }
}

String ModuleWifi::getNetwork() {
    wifi_mode_t wifiMode = WiFi.getMode();
    if (wifiMode == WIFI_AP) {
        return WIFI_NAME_AP;
    } else {
        return String(ModuleConfig::connConfig.wifi.ssid);
    }
}
