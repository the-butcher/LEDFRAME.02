#include "ModuleHttp.h"

#include <ArduinoJson.h>

AsyncWebServer ModuleHttp::server(80);
bool ModuleHttp::hasBegun = false;

void ModuleHttp::powerup() {
    if (!ModuleHttp::hasBegun) {
        wifi_mode_____e wifiMode = ModuleWifi::getClientState();
        if (wifiMode == WIFI_MODE_________AP || wifiMode == WIFI_MODE____STATION) {  // do not try to start http without wifi in station or ap mode
            // server.on("/cmnd/power", HTTP_GET, handleCommandPower);
            server.on("/stat/config", HTTP_GET, handleStatusConfig);
            server.on("/cmnd/config", HTTP_GET, handleCommandConfig);
            DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
            server.begin();
            ModuleHttp::hasBegun = true;
        }
    }
}

void ModuleHttp::depower() {
    if (ModuleHttp::hasBegun) {
        server.end();
        ModuleHttp::hasBegun = false;
    }
}

void ModuleHttp::handleCommandConfig(AsyncWebServerRequest *request) {

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    response->addHeader("Cache-Control", "max-age=60");
    JsonDocument jsonDocument;
    jsonDocument["code"] = 200;
    JsonObject wifiJo = jsonDocument["wifi"].to<JsonObject>();
    JsonObject mqttJo = jsonDocument["mqtt"].to<JsonObject>();

    bool hasValueUpdates = false;
    if (request->hasParam("ssid")) {
        String ssid = request->getParam("ssid")->value();
        if (ssid != String(ModuleConfig::connConfig.wifi.ssid)) {
            ssid.toCharArray(ModuleConfig::connConfig.wifi.ssid, 32);
            wifiJo["ssid"] = ModuleConfig::connConfig.wifi.ssid;
            hasValueUpdates = true;
        }
    }
    if (request->hasParam("wpwd")) {
        String wpwd = request->getParam("wpwd")->value();
        if (wpwd != String(ModuleConfig::connConfig.wifi.wpwd)) {
            wpwd.toCharArray(ModuleConfig::connConfig.wifi.wpwd, 32);
            wifiJo["wpwd"] = ModuleConfig::connConfig.wifi.wpwd;
            hasValueUpdates = true;
        }
    }
    if (request->hasParam("addr")) {
        String addr = request->getParam("addr")->value();
        if (addr != String(ModuleConfig::connConfig.mqtt.addr)) {
            addr.toCharArray(ModuleConfig::connConfig.mqtt.addr, 32);
            mqttJo["addr"] = ModuleConfig::connConfig.mqtt.addr;
            hasValueUpdates = true;
        }
    }
    if (request->hasParam("port")) {
        String portRaw = request->getParam("port")->value();
        if (ModuleHttp::isNumeric(portRaw)) {
            int port = portRaw.toInt();
            if (port != ModuleConfig::connConfig.mqtt.port) {
                ModuleConfig::connConfig.mqtt.port = port;
                mqttJo["port"] = ModuleConfig::connConfig.mqtt.port;
                hasValueUpdates = true;
            }
        }
    }
    if (request->hasParam("user")) {
        String user = request->getParam("user")->value();
        if (user != String(ModuleConfig::connConfig.mqtt.user)) {
            user.toCharArray(ModuleConfig::connConfig.mqtt.user, 32);
            mqttJo["user"] = ModuleConfig::connConfig.mqtt.user;
            hasValueUpdates = true;
        }
    }
    if (request->hasParam("mpwd")) {
        String mpwd = request->getParam("mpwd")->value();
        if (mpwd != String(ModuleConfig::connConfig.mqtt.mpwd)) {
            mpwd.toCharArray(ModuleConfig::connConfig.mqtt.mpwd, 32);
            mqttJo["mpwd"] = ModuleConfig::connConfig.mqtt.mpwd;
            hasValueUpdates = true;
        }
    }
    if (request->hasParam("topc")) {
        String topc = request->getParam("topc")->value();
        if (topc != String(ModuleConfig::connConfig.mqtt.topc)) {
            topc.toCharArray(ModuleConfig::connConfig.mqtt.topc, 32);
            mqttJo["topc"] = ModuleConfig::connConfig.mqtt.topc;
            hasValueUpdates = true;
        }
    }

    if (hasValueUpdates) {
        ModuleConfig::storeConfig(ModuleConfig::connConfig);
    }

    serializeJson(jsonDocument, *response);
    request->send(response);
}

void ModuleHttp::handleStatusConfig(AsyncWebServerRequest *request) {

    AsyncResponseStream *response = request->beginResponseStream("application/json");
    response->addHeader("Cache-Control", "max-age=60");
    JsonDocument jsonDocument;
    jsonDocument["code"] = 200;
    JsonObject wifiJo = jsonDocument["wifi"].to<JsonObject>();
    wifiJo["ssid"] = ModuleConfig::connConfig.wifi.ssid;
    wifiJo["wpwd"] = ModuleConfig::connConfig.wifi.wpwd;
    wifiJo["stat"] = ModuleWifi::getClientState();

    JsonObject mqttJo = jsonDocument["mqtt"].to<JsonObject>();
    mqttJo["addr"] = ModuleConfig::connConfig.mqtt.addr;
    mqttJo["port"] = ModuleConfig::connConfig.mqtt.port;
    mqttJo["user"] = ModuleConfig::connConfig.mqtt.user;
    mqttJo["mpwd"] = ModuleConfig::connConfig.mqtt.mpwd;
    mqttJo["topc"] = ModuleConfig::connConfig.mqtt.topc;
    mqttJo["stat"] = ModuleMqtt::getClientState();

    serializeJson(jsonDocument, *response);
    request->send(response);
}

// void ModuleHttp::handleCommandPower(AsyncWebServerRequest *request) {

//     // ModuleConfig::increaseExpiryByMinutes(1);

//     AsyncResponseStream *response = request->beginResponseStream("application/json");
//     response->addHeader("Cache-Control", "max-age=60");
//     JsonDocument jsonDocument;
//     jsonDocument["code"] = 200;
//     int minutesToAutoOff = 1;
//     if (request->hasParam("m")) {
//         String m = request->getParam("m")->value();
//         if (ModuleHttp::isNumeric(m)) {
//             minutesToAutoOff = m.toInt();
//             jsonDocument["m"] = minutesToAutoOff;
//         }
//     }
//     if (request->hasParam("p")) {
//         String p = request->getParam("p")->value();
//         if (p == "0") {
//             ModuleLed::setPower(POWER__OFF, minutesToAutoOff);
//             ModuleMqtt::publishState();
//             jsonDocument["p"] = POWER__OFF;
//         } else if (p == "1") {
//             ModuleLed::setPower(POWER__LOW, minutesToAutoOff);
//             ModuleMqtt::publishState();
//             jsonDocument["p"] = POWER__LOW;
//         } else if (p == "2") {
//             ModuleLed::setPower(POWER__MID, minutesToAutoOff);
//             ModuleMqtt::publishState();
//             jsonDocument["p"] = POWER__MID;
//         } else if (p == "3") {
//             ModuleLed::setPower(POWER_HIGH, minutesToAutoOff);
//             ModuleMqtt::publishState();
//             jsonDocument["p"] = POWER_HIGH;
//         } else {
//             jsonDocument["code"] = 400;
//             jsonDocument["p"] = "p must be one of [0, 1, 2, 3]";
//         }
//     } else {
//         jsonDocument["code"] = 400;
//         jsonDocument["p"] = "p must be present";
//     }

//     serializeJson(jsonDocument, *response);
//     request->send(response);
// }

bool ModuleHttp::isNumeric(String value) {
    for (uint8_t i = 0; i < value.length(); i++) {
        if (!isDigit(value.charAt(i))) {
            return false;
        }
    }
    return true;
}
