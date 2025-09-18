#include "modules/ModuleConfig.h"

conn__________t ModuleConfig::connConfig = {};

void ModuleConfig::powerup() {

    EEPROM.begin(sizeof(conn__________t) + 1);

    conn__________t _connConfig;
    uint8_t eepromFlag = EEPROM.read(0);
    if (eepromFlag == 1) {  // stored config expectable
        EEPROM.readBytes(1, &ModuleConfig::connConfig, sizeof(ModuleConfig::connConfig));
    } else {  // no stored config -> create a default config and store to eeprom
        String ssid = "your-wifi-ssid";
        ssid.toCharArray(connConfig.wifi.ssid, 32);
        String wpwd = "your-wifi-pass";
        wpwd.toCharArray(connConfig.wifi.wpwd, 32);
        String addr = "your-mqtt-ip";  // 192.168.0.38
        addr.toCharArray(connConfig.mqtt.addr, 32);
        connConfig.mqtt.port = 1883;
        String user = "your-mqtt-user";
        user.toCharArray(connConfig.mqtt.user, 32);
        String mpwd = "your-mqtt-pass";
        mpwd.toCharArray(connConfig.mqtt.mpwd, 32);
        String topc = "your-mqtt-topic";
        topc.toCharArray(connConfig.mqtt.topc, 32);
        ModuleConfig::storeConfig(connConfig);
    }

    ModuleMqtt::handleTopicUpdated();
}

void ModuleConfig::storeConfig(conn__________t _connConfig) {
#ifdef USE_NEOPIXEL
    for (uint8_t count = 0; count < 3; count++) {
        ModuleSignal::setPixelColor(COLOR______RED);
        delay(200);
        ModuleSignal::setPixelColor(COLOR____BLACK);
        delay(200);
    }
#endif
    EEPROM.write(0, 1);                                       // write eeprom flag
    EEPROM.writeBytes(1, &_connConfig, sizeof(_connConfig));  // write config
    EEPROM.commit();
}
