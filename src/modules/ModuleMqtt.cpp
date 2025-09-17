#include "ModuleMqtt.h"

#include <WiFiClientSecure.h>

#include "ModuleBattery.h"
#include "ModuleLed.h"
#include "ModuleWifi.h"

WiFiClient* ModuleMqtt::wifiClient;
PubSubClient* ModuleMqtt::mqttClient;
bool ModuleMqtt::propagatePower = false;
bool ModuleMqtt::hasBegun = false;
String ModuleMqtt::mqttPub = "";
String ModuleMqtt::mqttSub = "";

void ModuleMqtt::begin() {
    if (ModuleWifi::getClientState() == WIFI_MODE____STATION) {  //  do not try to start mqtt without wifi being in station mode
        if (!ModuleMqtt::hasBegun) {
            ModuleMqtt::wifiClient = new WiFiClient();
            ModuleMqtt::mqttClient = new PubSubClient(*ModuleMqtt::wifiClient);
            ModuleMqtt::mqttClient->setServer(ModuleConfig::connConfig.mqtt.addr, ModuleConfig::connConfig.mqtt.port);
            ModuleMqtt::mqttClient->setCallback(ModuleMqtt::handleMessage);
            ModuleMqtt::hasBegun = true;
        }
    }
}

void ModuleMqtt::connect() {
    if (ModuleMqtt::hasBegun) {
        if (String(ModuleConfig::connConfig.mqtt.user) != "" && String(ModuleConfig::connConfig.mqtt.mpwd) != "") {
#ifdef USE___SERIAL
            Serial.print("connecting with topic, user and password, ");
#endif
            mqttClient->connect(ModuleConfig::connConfig.mqtt.topc, ModuleConfig::connConfig.mqtt.user, ModuleConfig::connConfig.mqtt.mpwd, 0, 0, 0, 0, 0);
        } else {
#ifdef USE___SERIAL
            Serial.print("connecting with topic only");
#endif
            mqttClient->connect(ModuleConfig::connConfig.mqtt.topc);  // connect without credentials
        }
#ifdef USE___SERIAL
        Serial.println(ModuleMqtt::mqttClient->state());
        Serial.print("subscribing to: ");
        Serial.print(ModuleMqtt::mqttSub);
        Serial.print(", ");
        Serial.println(ModuleMqtt::mqttClient->state());
#endif
        ModuleMqtt::mqttClient->subscribe(ModuleMqtt::mqttSub.c_str());  // MQTT_SUB.c_str()
    }
}

void ModuleMqtt::handleTopicUpdated() {
    ModuleMqtt::mqttPub = "stat/" + String(ModuleConfig::connConfig.mqtt.topc) + "/Power";
    ModuleMqtt::mqttSub = "cmnd/" + String(ModuleConfig::connConfig.mqtt.topc) + "/Power";
    // TODO :: resubscribe (?, maybe the next awake/reconnect can be awaited)
    // if (ModuleMqtt::hasBegun) {
    //     ModuleMqtt::mqttClient->subscribe(ModuleMqtt::mqttSub.c_str());
    // }
}

void ModuleMqtt::loop() {
    if (ModuleMqtt::hasBegun) {
        if (!ModuleMqtt::mqttClient->connected()) {
            ModuleMqtt::connect();
        }
        ModuleMqtt::mqttClient->loop();
    }
}

void ModuleMqtt::prepareSleep() {
    if (ModuleMqtt::hasBegun) {
        ModuleMqtt::mqttClient->disconnect();            // calls stop() on wificlient
        while (ModuleMqtt::mqttClient->state() != -1) {  // https://github.com/knolleary/pubsubclient/issues/452
            delay(10);                                   // wait for disconnect (will flush the last message, if present)
        }
        ModuleMqtt::wifiClient->stop();  // explicit stop to be sure it happened (before resetting certFileData)
        delete ModuleMqtt::mqttClient;   // releases some memory buffer
        delete ModuleMqtt::wifiClient;   // calls stop (again) and deletes an internal sslclient instance
        ModuleMqtt::mqttClient = NULL;
        ModuleMqtt::wifiClient = NULL;
        ModuleMqtt::hasBegun = false;
    }
}

void ModuleMqtt::setPropagatePower(bool isPropagatePower) {
    ModuleMqtt::propagatePower = isPropagatePower;
}

bool ModuleMqtt::isPropagatePower() {
    return ModuleMqtt::propagatePower;
}

mqtt_mode_____e ModuleMqtt::getClientState() {
    if (ModuleMqtt::hasBegun) {
        int state = ModuleMqtt::mqttClient->state();
        if (state == MQTT_CONNECTION_TIMEOUT) {  // -4
            return MQTT_TIMEOUT____CONN;
        } else if (state == MQTT_CONNECTION_LOST) {  // -3
            return MQTT_LOST_______CONN;
        } else if (state == MQTT_CONNECT_FAILED) {  // -2
            return MQTT_FAIL_______CONN;
        } else if (state == MQTT_DISCONNECTED) {  // -1
            return MQTT_LOST_______CONN;
        } else if (state == MQTT_CONNECTED) {  // 0 :: OK
            return MQTT______________OK;
        } else if (state == MQTT_CONNECT_BAD_PROTOCOL) {  // 1
            return MQTT_BAD____PROTOCOL;
        } else if (state == MQTT_CONNECT_BAD_CLIENT_ID) {  // 2
            return MQTT_BAD_________CLI;
        } else if (state == MQTT_CONNECT_UNAVAILABLE) {  // 3
            return MQTT_UNAVAIL____CONN;
        } else if (state == MQTT_CONNECT_BAD_CREDENTIALS) {  // 4
            return MQTT_BAD_CREDENTIALS;
        } else if (state == MQTT_CONNECT_UNAUTHORIZED) {  // 5
            return MQTT_NO_________AUTH;
        } else {
            return MQTT_________UNKNOWN;
        }
    } else {
        return MQTT_NO__________CLI;
    }
}

void ModuleMqtt::handleMessage(char* topic, byte* payload, unsigned int length) {

    // ModuleConfig::increaseExpiryByMinutes(1);  // if there was one mqtt command, expect more to come -> leave connection upright

    if (ModuleMqtt::mqttSub == String(topic)) {
        JsonDocument doc;
        deserializeJson(doc, payload, length);
        int minutesToAutoOff = 1;
        if (!doc["m"].isNull() && doc["m"].is<int>()) {
            minutesToAutoOff = doc["m"].as<int>();
        }
        if (!doc["p"].isNull() && doc["p"].is<int>()) {
            int pi = doc["p"].as<int>();
            pi = min((int)POWER_HIGH, pi);
            pi = max((int)POWER__OFF, pi);
            ModuleLed::setPower((power_t)pi, minutesToAutoOff);
            ModuleMqtt::publishState();
            ModuleMqtt::clearCommand();
        }
    }
}

/**
 * https://arduinojson.org/v7/how-to/use-arduinojson-with-pubsubclient/
 */
void ModuleMqtt::publishState() {
    if (ModuleMqtt::hasBegun) {
        if (!ModuleMqtt::mqttClient->connected()) {
            ModuleMqtt::connect();
        }
        JsonDocument jsonDocument;
        jsonDocument["lp"] = ModuleLed::getPower();
        jsonDocument["bp"] = ModuleBattery::getPercentage();
        jsonDocument["ip"] = ModuleWifi::getAddress();
        char outputBuf[128];
        serializeJson(jsonDocument, outputBuf);
        ModuleMqtt::mqttClient->publish(ModuleMqtt::mqttPub.c_str(), outputBuf, true);
    }
}

void ModuleMqtt::clearCommand() {
    if (ModuleMqtt::hasBegun) {
        if (!ModuleMqtt::mqttClient->connected()) {
            ModuleMqtt::connect();
        }
        char outputBuf[0];
        ModuleMqtt::mqttClient->publish(ModuleMqtt::mqttSub.c_str(), outputBuf, true);
    }
}