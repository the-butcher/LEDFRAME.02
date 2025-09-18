#ifndef ModuleTouch_h
#define ModuleTouch_h

#include <Arduino.h>
#include <driver/rtc_io.h>

#include "ModuleLed.h"
#include "ModuleMqtt.h"
#include "ModuleSignal.h"
#include "ModuleWifi.h"

typedef enum : uint8_t {
    TOUCH_MODE______NONE,
    TOUCH_MODE_____SHORT,
    TOUCH_MODE______LONG
} touch_mode____e;

const gpio_num_t PIN_____TOUCH = GPIO_NUM_8;
const int MINUTES_TO_AUTO_OFF_TOUCH = 2;

class ModuleTouch {
   private:
    static uint64_t millisDown;
    static touch_mode____e touchStatus;

   public:
    static void powerup(bool isTouchBegin);
    static void loop();
    static void depower();
    static void handleButtonChange();
    static bool isTouched();
    static bool isReadyToSleep();
};

#endif