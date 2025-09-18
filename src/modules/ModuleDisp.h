#ifndef ModuleDisp_h
#define ModuleDisp_h

#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <qrcode.h>

#include "modules/ModuleBattery.h"
#include "modules/ModuleWifi.h"
#include "px_font_5x7.h"

class ModuleDisp {
   private:
    static bool hasBegun;
    static GxEPD2_BW<GxEPD2_102, GxEPD2_102::HEIGHT> baseDisplay;
    static bool isActive;

   public:
    static void powerup();
    static void depower();
    static void renderBattery(void* parameter);
    static void renderStatWifi(void* parameter);
    static String formatString(String value, char const* format);
    static bool isReadyToSleep();
};

#endif