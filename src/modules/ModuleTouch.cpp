#include "modules/ModuleTouch.h"

uint64_t ModuleTouch::millisDown = 0;
touch_mode____e ModuleTouch::touchStatus = TOUCH_MODE______NONE;

void ModuleTouch::powerup(bool isTouchBegin) {
    rtc_gpio_deinit(PIN_____TOUCH);
    pinMode(PIN_____TOUCH, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_____TOUCH), ModuleTouch::handleButtonChange, CHANGE);
    if (isTouchBegin) {  // if it woke up from a button press, update state to
        ModuleTouch::millisDown = millis();
        ModuleTouch::handleButtonChange();  // can set touchStatus to TOUCH__________SHORT
        ModuleTouch::loop();                // handles TOUCH__________SHORT if set
    }
}

bool ModuleTouch::isReadyToSleep() {
    return !ModuleTouch::isTouched();
}

bool ModuleTouch::isTouched() {
    return digitalRead(PIN_____TOUCH) == LOW;
}

void ModuleTouch::handleButtonChange() {
    if (ModuleTouch::isTouched()) {
        ModuleTouch::millisDown = millis();
    } else if (ModuleTouch::millisDown > 0) {  // regular button release, a long button press would be detected in ModuleTouch::loop()
        ModuleTouch::touchStatus = TOUCH_MODE_____SHORT;
        ModuleTouch::millisDown = 0;
    }
}

void ModuleTouch::depower() {
    detachInterrupt(digitalPinToInterrupt(PIN_____TOUCH));
    esp_sleep_enable_ext0_wakeup(PIN_____TOUCH, LOW);
    gpio_hold_en(PIN_____TOUCH);
    rtc_gpio_pullup_en(PIN_____TOUCH);
    rtc_gpio_pulldown_dis(PIN_____TOUCH);
}

void ModuleTouch::loop() {

    // check for long button press
    if (ModuleTouch::millisDown > 0 && millis() > ModuleTouch::millisDown + 3000) {
        ModuleTouch::touchStatus = TOUCH_MODE______LONG;
        ModuleTouch::millisDown = 0;
    }

#ifdef USE___SERIAL
    if (ModuleTouch::touchStatus != TOUCH_MODE______NONE) {
        Serial.print("touch mode: ");
        Serial.println(ModuleTouch::touchStatus);
    }
#endif

    if (ModuleTouch::touchStatus == TOUCH_MODE______LONG) {
        ModuleSignal::setPixelColor(COLOR__MAGENTA);
        ModuleMqtt::depower();                      // disconnect mqtt
        ModuleHttp::depower();                      // stop http server
        ModuleWifi::depower();                      // disconnect wifi
        ModuleWifi::powerup(WIFI_MODE_________AP);  // restart in AP mode
        ModuleHttp::powerup();                      // start http server
        xTaskCreatePinnedToCore(ModuleDisp::renderStatWifi, "renderStatWifi", 7500, NULL, 1, NULL, 0);
        ModuleTouch::touchStatus = TOUCH_MODE______NONE;  // treat as handled
    } else if (ModuleTouch::touchStatus == TOUCH_MODE_____SHORT) {
        wifi_mode_____e wifiMode = ModuleWifi::getClientState();
        if (wifiMode == WIFI_MODE_________AP) {
            ModuleSignal::setPixelColor(COLOR_____BLUE);
            ModuleHttp::depower();  // stop http server
            ModuleWifi::depower();  // disconnect wifi
            for (uint8_t i = 0; i < 50; i++) {
                delay(100);
                if (!ModuleWifi::isConnected()) {
                    ESP.restart();
                }
            }
        } else {
            power_t power = (power_t)(ModuleLed::getPower() + 1);
            if (power > POWER_HIGH) {
                power = POWER__OFF;
            }
            ModuleLed::setPower(power, MINUTES_TO_AUTO_OFF_TOUCH);
            ModuleMqtt::publishState();
            ModuleTouch::touchStatus = TOUCH_MODE______NONE;  // treat as handled
        }
    }
}
