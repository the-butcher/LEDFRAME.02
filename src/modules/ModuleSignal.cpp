#include "modules/ModuleSignal.h"

#ifdef USE_NEOPIXEL
#include <Adafruit_NeoPixel.h>
#endif

#ifdef USE_NEOPIXEL
Adafruit_NeoPixel pixels(1, GPIO_NUM_33, NEO_GRB + NEO_KHZ800);
#endif

color_t ModuleSignal::pixelColor = COLOR____BLACK;

void ModuleSignal::begin() {
#ifdef USE_NEOPIXEL
    pixels.begin();
#endif
}

void ModuleSignal::prepareSleep() {
    pinMode(NEOPIXEL_POWER, OUTPUT);
    digitalWrite(NEOPIXEL_POWER, LOW);
    gpio_hold_dis((gpio_num_t)NEOPIXEL_POWER);
    pinMode(I2C_POWER, OUTPUT);
    digitalWrite(I2C_POWER, LOW);
    gpio_hold_dis((gpio_num_t)I2C_POWER);
}

color_t ModuleSignal::getPixelColor() {
    return ModuleSignal::pixelColor;
}

void ModuleSignal::setPixelColor(color_t pixelColor) {
    ModuleSignal::pixelColor = pixelColor;
#ifdef USE_NEOPIXEL
    pixels.setPixelColor(0, ModuleSignal::pixelColor);  // red for measuring
    pixels.show();
#endif
}