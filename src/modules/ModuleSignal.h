#ifndef ModuleSignal_h
#define ModuleSignal_h

#include <Arduino.h>
#include <driver/rtc_io.h>

#include "types/Define.h"

typedef enum : uint32_t {
    COLOR____WHITE = 0x060606,
    COLOR______RED = 0x060000,
    COLOR___ORANGE = 0x060C00,
    COLOR____GREEN = 0x000600,
    COLOR___YELLOW = 0x060600,
    COLOR_____BLUE = 0x000006,
    COLOR_____CYAN = 0x000606,
    COLOR__MAGENTA = 0x060006,
    COLOR____BLACK = 0x000000
} color_t;

class ModuleSignal {
   private:
    static color_t pixelColor;

   public:
    static void begin();
    static void prepareSleep();
    static color_t getPixelColor();
    static void setPixelColor(color_t pixelColor);
};

#endif