#include <Arduino.h>

#include "modules/ModuleBattery.h"
#include "modules/ModuleConfig.h"
#include "modules/ModuleDisp.h"
#include "modules/ModuleHttp.h"
#include "modules/ModuleLed.h"
#include "modules/ModuleMqtt.h"
#include "modules/ModuleSignal.h"
#include "modules/ModuleTouch.h"
#include "modules/ModuleWifi.h"
#include "types/Define.h"

RTC_DATA_ATTR uint64_t bootCount = 0;
RTC_DATA_ATTR uint8_t wifiFailCount = 0;
RTC_DATA_ATTR uint8_t mqttFailCount = 0;

void depowerConn() {
#ifndef USE___SERIAL
    ModuleSignal::setPixelColor(COLOR_____CYAN);
    ModuleMqtt::depower();
    ModuleHttp::depower();
    ModuleWifi::depower();
    ModuleSignal::setPixelColor(COLOR____BLACK);
#endif
}

bool hasExceededMaxConnectionAttempts() {
    return wifiFailCount > 4 || mqttFailCount > 4;
}

void powerupConn() {
    if (!hasExceededMaxConnectionAttempts()) {  // further connection attempts allowed?
        ModuleWifi::powerup(WIFI_MODE____STATION);
        if (ModuleWifi::getClientState() != WIFI_MODE____STATION) {  // connection failure
            wifiFailCount++;
#ifdef USE_NEOPIXEL
            ModuleSignal::setPixelColor(COLOR___ORANGE);
            delay(250);
#endif
        } else {
            wifiFailCount = 0;
        }
        ModuleMqtt::powerup();  // will not begin when wifi is not in station mode
        ModuleMqtt::loop();     // do a single mqtt loop to see if it can connect
        if (ModuleMqtt::getClientState() != MQTT______________OK) {
            mqttFailCount++;
#ifdef USE_NEOPIXEL
            ModuleSignal::setPixelColor(COLOR___ORANGE);
            delay(250);
#endif
        } else {
            mqttFailCount = 0;
        }
    } else {  // no further connection attempts
#ifdef USE_NEOPIXEL
        ModuleSignal::setPixelColor(COLOR______RED);
        delay(250);
#endif
    }
}

uint8_t getTimerWakeupMinutes() {
    if (bootCount == 1) {  // clear the wifi info display upon first wakeup
        return 1;
    } else {
        return hasExceededMaxConnectionAttempts() ? WAKEUP_MINUTES_NO_CONN : WAKEUP_MINUTES____CONN;
    }
}

uint64_t getTimerWakeupMicros() {
    return MICROSECONDS_PER_MINUTE * getTimerWakeupMinutes();
}

void initializeDeepSleep() {
#ifndef USE___SERIAL
    ModuleSignal::setPixelColor(COLOR_____BLUE);
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    esp_sleep_enable_timer_wakeup(getTimerWakeupMicros());  // wake up in short intervals while wifi connection promises success
    ModuleDisp::depower();
    ModuleLed::depower();
    ModuleTouch::depower();
    depowerConn();
    ModuleSignal::setPixelColor(COLOR_____BLUE);
    ModuleSignal::depower();
    ModuleBattery::depower();  // nothing
    esp_deep_sleep_start();
#endif
}

void setup() {

#ifdef USE___SERIAL
    Serial.begin(115200);
    delay(2000);
#endif

    ModuleConfig::powerup();

    ModuleBattery::powerup();
    delay(100);

    ModuleSignal::powerup();
    ModuleSignal::setPixelColor(COLOR____GREEN);

    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    // ESP_SLEEP_WAKEUP_TIMER, ESP_SLEEP_WAKEUP_EXT0

    ModuleLed::powerup();

    ModuleTouch::powerup(wakeup_reason == ESP_SLEEP_WAKEUP_EXT0);

    powerupConn();

    uint8_t bootCountRedrawRatio = WAKEUP_MINUTES_NO_CONN / getTimerWakeupMinutes();  // 1 in case of WAKEUP_MINUTES_NO_CONN minute interval, WAKEUP_MINUTES_NO_CONN in case of WAKEUP_MINUTES____CONN minute interval
                                                                                      // main task is running on core 1
    if (bootCount == 0) {
        xTaskCreatePinnedToCore(ModuleDisp::renderStatWifi, "renderStatWifi", 7500, NULL, 1, NULL, 0);
    } else if (bootCount == 1 || bootCount % bootCountRedrawRatio == 0) {
        xTaskCreatePinnedToCore(ModuleDisp::renderBattery, "renderBattery", 7500, NULL, 1, NULL, 0);
    }
    bootCount++;

    ModuleSignal::setPixelColor(COLOR____BLACK);
}

void loop() {

    // 250 ms with connectivity
    for (uint8_t i = 0; i < 25; i++) {
        ModuleMqtt::loop();
        ModuleTouch::loop();
        delay(10);
    }
    ModuleLed::loop();

    if (ModuleWifi::isReadyToSleep()) {
        depowerConn();
    }

    // 10000ms without connectivity (40 cycles of 250 ms)
    for (uint8_t i = 0; i < 40; i++) {

        for (uint8_t j = 0; j < 25; j++) {
            ModuleTouch::loop();
            delay(10);
        }
        ModuleLed::loop();

        // deep sleep at the earliest possible time (LEDs off, display inactive, button not held down, wifi not in AP mode)
        if (ModuleLed::isReadyToSleep() && ModuleDisp::isReadyToSleep() && ModuleTouch::isReadyToSleep() && ModuleWifi::isReadyToSleep()) {
            initializeDeepSleep();
        }
    }

    if (ModuleWifi::isReadyToSleep()) {
        powerupConn();  // reconnect wifi and mqtt (if not already connected) for the next ~10s cycle
    }
};