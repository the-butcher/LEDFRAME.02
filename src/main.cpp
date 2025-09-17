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

void initializeConnSleep() {
#ifndef USE___SERIAL
    ModuleMqtt::prepareSleep();
    ModuleHttp::prepareSleep();
    ModuleWifi::prepareSleep();
#endif
}

bool hasExceededMaxConnectionAttempts() {
    return wifiFailCount > 3 || mqttFailCount > 3;
}

uint8_t getTimerWakeupMinutes() {
    if (bootCount == 1) {
        return 1;
    } else {
        return hasExceededMaxConnectionAttempts() ? 30 : 1;
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
    ModuleDisp::prepareSleep();
    ModuleLed::prepareSleep();
    ModuleTouch::prepareSleep();
    initializeConnSleep();
    ModuleSignal::prepareSleep();
    ModuleBattery::prepareSleep();  // nothing
    esp_deep_sleep_start();
#endif
}

void setup() {

#ifdef USE___SERIAL
    Serial.begin(115200);
    delay(2000);
#endif

    ModuleConfig::begin();

    ModuleBattery::begin();
    delay(100);

    ModuleSignal::begin();
    ModuleSignal::setPixelColor(COLOR____GREEN);

    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    // if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) {  // ESP_SLEEP_WAKEUP_EXT0
    // }

    ModuleLed::begin();

    ModuleTouch::begin(wakeup_reason == ESP_SLEEP_WAKEUP_EXT0);

    if (!hasExceededMaxConnectionAttempts()) {  // further connection attempts allowed?
        ModuleWifi::begin(WIFI_MODE____STATION);
        if (ModuleWifi::getClientState() != WIFI_MODE____STATION) {  // connection failure
            wifiFailCount++;
            ModuleSignal::setPixelColor(COLOR___ORANGE);
            delay(500);
        }
        ModuleMqtt::begin();  // will not begin when wifi is not in station mode
        ModuleMqtt::loop();   // do a single mqtt loop to see if it can connect
        if (ModuleMqtt::getClientState() != MQTT______________OK) {
            mqttFailCount++;
            ModuleSignal::setPixelColor(COLOR___ORANGE);
            delay(500);
        }
    } else {  // no further connection attempts
        ModuleSignal::setPixelColor(COLOR______RED);
        delay(500);
    }

    uint8_t bootCountRedrawRatio = 30 / getTimerWakeupMinutes();  // 1 in case of 30 minute interval, 30 in case of 1 minute interval
    if (bootCount == 0) {
        xTaskCreatePinnedToCore(ModuleDisp::renderStatWifi, "renderStatWifi", 7500, NULL, 1, NULL, 0);
    } else if (bootCount == 1 || bootCount % bootCountRedrawRatio == 0) {
        // main task is running on core 1
        xTaskCreatePinnedToCore(ModuleDisp::renderBattery, "renderBattery", 7500, NULL, 1, NULL, 0);
    }
    bootCount++;

    ModuleSignal::setPixelColor(COLOR____BLACK);
}

void loop() {

    for (uint8_t i = 0; i < 25; i++) {
        ModuleMqtt::loop();
        ModuleTouch::loop();
        delay(10);
    }
    ModuleLed::loop();  // check for auto off, happens ~once per second

    if (!ModuleMqtt::isPropagatePower()) {
        ModuleMqtt::publishState();  // late publish for touch power on
    }
    ModuleMqtt::setPropagatePower(true);

    if (ModuleWifi::isReadyToSleep()) {
        if (ModuleLed::isReadyToSleep() && ModuleDisp::isReadyToSleep() && ModuleTouch::isReadyToSleep()) {
            initializeDeepSleep();
        } else {
            initializeConnSleep();
        }
    } else {
        ModuleHttp::begin();  // turn http on only if LEDs actually came on, // will not begin when wifi is not in ap or station mode
    }
};