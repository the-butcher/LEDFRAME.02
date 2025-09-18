#ifndef ModuleBattery_h
#define ModuleBattery_h

#include <Adafruit_MAX1704X.h>
#include <Arduino.h>

class ModuleBattery {
   private:
    static Adafruit_MAX17048 baseBattery;

   public:
    static void powerup();
    static void depower();
    static float getPercentage();
};

#endif