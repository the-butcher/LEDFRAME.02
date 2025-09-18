#ifndef ModuleConfig_h
#define ModuleConfig_h

#include <Arduino.h>
#include <EEPROM.h>

#include "modules/ModuleMqtt.h"
#include "modules/ModuleSignal.h"
#include "types/Define.h"

typedef struct {
    char ssid[32];
    char wpwd[32];
} wifi__________t;

typedef struct {
    char addr[32];
    uint16_t port;
    char user[32];
    char mpwd[32];
    char topc[32];
} mqtt__________t;

typedef struct {
    wifi__________t wifi;
    mqtt__________t mqtt;
} conn__________t;

class ModuleConfig {
   private:
   public:
    static void powerup();
    static conn__________t connConfig;
    static void storeConfig(conn__________t connConfig);
};

#endif