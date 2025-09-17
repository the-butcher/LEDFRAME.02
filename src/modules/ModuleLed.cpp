#include "modules/ModuleLed.h"

#include <Arduino.h>

#include "ModuleMqtt.h"

power_t ModuleLed::power = POWER__OFF;
uint64_t ModuleLed::autoOffMillis = 0;

void ModuleLed::begin() {
    pinMode(PIN_____PWM____LED, OUTPUT);
    ledcSetup(CHANNEL_PWM____LED, 1000, 8);  // TODO :: research appropriate LED PWM frequency
    ledcAttachPin(PIN_____PWM____LED, CHANNEL_PWM____LED);
}

void ModuleLed::prepareSleep() {
    ModuleLed::setPower(POWER__OFF, 1);
}

power_t ModuleLed::getPower() {
    return ModuleLed::power;
}

uint64_t ModuleLed::getMillisToAutoOff() {
    if (millis() < ModuleLed::autoOffMillis) {
        return ModuleLed::autoOffMillis - millis();
    } else {
        return 0;
    }
}

void ModuleLed::setPower(power_t power, int minutesToAutoOff) {
    if (power != ModuleLed::power && power >= POWER__OFF && power <= POWER_HIGH) {
        ModuleLed::power = power;
        ModuleLed::autoOffMillis = millis() + minutesToAutoOff * MILLISECONDS_PER_MINUTE;  // should not be a problem when power == POWER__OFF because the variable will be lost when going to sleep anyways
        ledcWrite(CHANNEL_PWM____LED, stateLevels[ModuleLed::power]);
    }
}

void ModuleLed::loop() {
    if (ModuleLed::getMillisToAutoOff() == 0 && ModuleLed::power != POWER__OFF) {
        ModuleLed::setPower(POWER__OFF, 1);
        ModuleMqtt::publishState();
    }
}

bool ModuleLed::isReadyToSleep() {
    return ModuleLed::power == POWER__OFF;
}