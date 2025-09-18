#include "modules/ModuleBattery.h"

#include <Arduino.h>

Adafruit_MAX17048 ModuleBattery::baseBattery;

void ModuleBattery::powerup() {
    ModuleBattery::baseBattery.begin();
}

void ModuleBattery::depower() {
    // ModuleBattery::baseBattery.hibernate();
}

float ModuleBattery::getPercentage() {
    return ModuleBattery::baseBattery.cellPercent();
}
