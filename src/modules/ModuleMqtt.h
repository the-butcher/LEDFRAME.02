#ifndef ModuleMqtt_h
#define ModuleMqtt_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

#include "modules/ModuleConfig.h"

typedef enum : uint8_t {
    MQTT______________OK = 10,
    // connection issues, associated with states of the mqtt client after a connection attempt
    MQTT_TIMEOUT____CONN = 20,
    MQTT_LOST_______CONN = 21,  // connection was lost
    MQTT_FAIL_______CONN = 22,  // connect failed
    MQTT_FAIL____PUBLISH = 30,  // failure during publishing
    // unrecoverable issues
    MQTT_BAD____PROTOCOL = 40,
    MQTT_BAD_________CLI = 41,
    MQTT_UNAVAIL____CONN = 42,
    MQTT_BAD_CREDENTIALS = 43,
    MQTT_NO_________AUTH = 44,
    // configuration issues, unrecoverable
    MQTT_CNF____NOT_USED = 50,
    MQTT_CNF_________SRV = 60,
    MQTT_CNF_________PRT = 61,
    MQTT_CNF_________CLI = 62,
    MQTT_FAIL________DAT = 63,
    MQTT_NO__________DAT = 64,
    MQTT_NO__________CLI = 70,
    // unknown issues
    MQTT_________UNKNOWN = 99,

} mqtt_mode_____e;

class ModuleMqtt {
   private:
    static WiFiClient* wifiClient;
    static PubSubClient* mqttClient;
    static bool propagatePower;
    static bool hasBegun;
    static String mqttPub;
    static String mqttSub;

   public:
    static void begin();
    static void prepareSleep();
    static void connect();
    static void loop();
    static void setPropagatePower(bool isPropagatePower);
    static bool isPropagatePower();
    static void publishState();
    static void clearCommand();
    static mqtt_mode_____e getClientState();
    static void handleMessage(char* topic, byte* payload, unsigned int length);
    static void handleTopicUpdated();
};

#endif