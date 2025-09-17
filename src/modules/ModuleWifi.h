#ifndef ModuleWifi_h
#define ModuleWifi_h

#include <Arduino.h>
#include <WiFi.h>

#include "modules/ModuleConfig.h"
#include "modules/ModuleDisp.h"
#include "modules/ModuleHttp.h"
#include "modules/ModuleMqtt.h"
#include "types/Define.h"

// 192.168.0.89

const String WIFI_NAME_AP = "LEDFRAME";

typedef enum : uint8_t {
    WIFI_MODE________OFF = 0,
    WIFI_MODE____STATION = 10,
    WIFI_MODE_________AP = 11
} wifi_mode_____e;

class ModuleWifi {
   private:
    static void handleStationDisconnected(WiFiEvent_t event);

   public:
    static void begin(wifi_mode_____e mode);
    static void prepareSleep();
    static bool isConnected();
    static String getNetwork();
    static String getAddress();
    static wifi_mode_____e getClientState();
    static bool isReadyToSleep();
};

#endif