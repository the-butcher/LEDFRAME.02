#ifndef ModuleLed_h
#define ModuleLed_h

#include <Arduino.h>

#include "modules/ModuleTouch.h"
#include "types/Define.h"

const int CHANNEL_PWM____LED = 0;
const gpio_num_t PIN_____PWM____LED = GPIO_NUM_18;

typedef enum : uint8_t {
    POWER__OFF = 0,
    POWER__LOW = 1,
    POWER__MID = 2,
    POWER_HIGH = 3
} power_t;

const uint8_t stateLevels[] = {0, 50, 150, 250};

class ModuleLed {
   private:
    static power_t power;
    static uint64_t autoOffMillis;

   public:
    static void powerup();
    static void loop();
    static void depower();
    static power_t getPower();
    static void setPower(power_t power, int minutesToAutoOff);
    static bool isReadyToSleep();
    static uint64_t getMillisToAutoOff();
};

#endif