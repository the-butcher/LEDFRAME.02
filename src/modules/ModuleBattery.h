#ifndef ModuleBattery_h
#define ModuleBattery_h

#include <Adafruit_MAX1704X.h>
#include <Arduino.h>

class ModuleBattery {
   private:
    static Adafruit_MAX17048 baseBattery;

   public:
    static void begin();
    static void prepareSleep();
    static float getPercentage();
};

#endif