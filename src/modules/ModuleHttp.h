#ifndef ModuleHttp_h
#define ModuleHttp_h

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#include "ModuleConfig.h"
#include "ModuleLed.h"
#include "modules/ModuleMqtt.h"
#include "modules/ModuleWifi.h"

class ModuleHttp {
   private:
    static AsyncWebServer server;
    static bool hasBegun;

   public:
    static void begin();
    static void prepareSleep();
    static void handleCommandPower(AsyncWebServerRequest *request);
    static void handleStatusConfig(AsyncWebServerRequest *request);
    static void handleCommandConfig(AsyncWebServerRequest *request);
    static bool isNumeric(String value);
};

#endif