#include "modules/ModuleBattery.h"

#include <Arduino.h>

Adafruit_MAX17048 ModuleBattery::baseBattery;

void ModuleBattery::begin() {
    ModuleBattery::baseBattery.begin();
}

void ModuleBattery::prepareSleep() {
    // nothing
}

float ModuleBattery::getPercentage() {
    return ModuleBattery::baseBattery.cellPercent();
}
