#include "ModuleDisp.h"

#include <ArduinoJson.h>

const char FORMAT_CELL_PERCENT[] = "%4s%%";

bool ModuleDisp::hasBegun = false;
bool ModuleDisp::isActive = false;
GxEPD2_BW<GxEPD2_102, GxEPD2_102::HEIGHT> ModuleDisp::baseDisplay(GxEPD2_102(GPIO_NUM_14, GPIO_NUM_15, GPIO_NUM_16, GPIO_NUM_17));

void ModuleDisp::powerup() {
    if (!ModuleDisp::hasBegun) {
        ModuleDisp::baseDisplay.init(115200, true, 2, false);
        ModuleDisp::hasBegun = true;
    }
}

void ModuleDisp::depower() {
    if (ModuleDisp::hasBegun) {
        ModuleDisp::baseDisplay.hibernate();
        pinMode(I2C_POWER, OUTPUT);  // display is wired to I2C power which is not needed otherwise, and it can be turned off
        digitalWrite(I2C_POWER, LOW);
        gpio_hold_dis((gpio_num_t)I2C_POWER);
    }
}

String ModuleDisp::formatString(String value, char const* format) {
    char padBuffer[8];
    sprintf(padBuffer, format, value);
    return padBuffer;
}

void ModuleDisp::renderStatWifi(void* parameter) {

    while (ModuleDisp::isActive) {
        vTaskDelay(100);
    }

    ModuleDisp::isActive = true;
    ModuleDisp::powerup();

    ModuleDisp::baseDisplay.setRotation(3);
    ModuleDisp::baseDisplay.setFont(&px_5_7);
    ModuleDisp::baseDisplay.setTextColor(GxEPD_BLACK);

    ModuleDisp::baseDisplay.setFullWindow();
    ModuleDisp::baseDisplay.firstPage();

    wifi_mode_____e wifiMode = ModuleWifi::getClientState();

    do {

        ModuleDisp::baseDisplay.fillScreen(GxEPD_WHITE);

        ModuleDisp::baseDisplay.setCursor(10, 10);
        ModuleDisp::baseDisplay.print("SSID: ");
        ModuleDisp::baseDisplay.setCursor(10, 17);
        ModuleDisp::baseDisplay.print(ModuleWifi::getNetwork());

        ModuleDisp::baseDisplay.setCursor(10, 27);
        ModuleDisp::baseDisplay.print("IP: ");
        ModuleDisp::baseDisplay.setCursor(10, 34);
        ModuleDisp::baseDisplay.print(ModuleWifi::getAddress());

        ModuleDisp::baseDisplay.setCursor(10, 44);
        ModuleDisp::baseDisplay.print("WIFI: ");
        ModuleDisp::baseDisplay.setCursor(10, 51);
        ModuleDisp::baseDisplay.print(ModuleWifi::getClientState());

        ModuleDisp::baseDisplay.setCursor(10, 61);
        ModuleDisp::baseDisplay.print("MQTT: ");
        ModuleDisp::baseDisplay.setCursor(10, 68);
        ModuleDisp::baseDisplay.print(ModuleMqtt::getClientState());

        if (wifiMode == WIFI_MODE_________AP) {
            String address = "http://" + ModuleWifi::getAddress() + "/stat/config";
            char addressBuf[address.length() + 1];
            address.toCharArray(addressBuf, address.length() + 1);
            int qrCodeX = 85;
            int qrCodeY = 37;
            QRCode qrcodeAddress;
            uint8_t qrcodeDataAddress[qrcode_getBufferSize(3)];
            qrcode_initText(&qrcodeAddress, qrcodeDataAddress, 3, 0, addressBuf);
            for (uint8_t y = 0; y < qrcodeAddress.size; y++) {
                for (uint8_t x = 0; x < qrcodeAddress.size; x++) {
                    if (qrcode_getModule(&qrcodeAddress, x, y)) {
                        baseDisplay.fillRect(x + qrCodeX, y + qrCodeY, 1, 1, GxEPD_BLACK);
                    }
                }
            }
        }

    } while (ModuleDisp::baseDisplay.nextPage());

    ModuleDisp::isActive = false;
    vTaskDelete(NULL);
}

void ModuleDisp::renderBattery(void* parameter) {

    while (ModuleDisp::isActive) {
        vTaskDelay(100);
    }

    ModuleDisp::isActive = true;

    ModuleDisp::powerup();

    ModuleDisp::baseDisplay.setRotation(0);
    ModuleDisp::baseDisplay.setFont(&px_5_7);

    ModuleDisp::baseDisplay.setFullWindow();
    ModuleDisp::baseDisplay.firstPage();

    uint8_t rectDimY = 17;
    uint8_t dispMidX = 37;
    uint8_t dispMidY = 66;

    uint8_t numBar = max(0, min(5, (int)round(ModuleBattery::getPercentage() / 20.0)));
    ModuleDisp::baseDisplay.setTextColor(GxEPD_WHITE);

    String cellPercentFormatted = ModuleDisp::formatString(String(ModuleBattery::getPercentage(), 0), FORMAT_CELL_PERCENT);
    int16_t tbx, tby;
    uint16_t tbw, tbh;
    ModuleDisp::baseDisplay.getTextBounds(cellPercentFormatted, 0, 0, &tbx, &tby, &tbw, &tbh);

    do {

        ModuleDisp::baseDisplay.fillScreen(GxEPD_WHITE);

        // black border, battery pole
        ModuleDisp::baseDisplay.fillRoundRect(dispMidX - 30, dispMidY - 45, 60, 99, 12, GxEPD_BLACK);
        ModuleDisp::baseDisplay.fillRoundRect(dispMidX - 15, dispMidY - 54, 30, 8, 8, GxEPD_BLACK);
        ModuleDisp::baseDisplay.fillRect(dispMidX - 15, dispMidY - 50, 30, 5, GxEPD_BLACK);

        // white inner border
        ModuleDisp::baseDisplay.fillRoundRect(dispMidX - 26, dispMidY - 41, 52, 91, 8, GxEPD_WHITE);

        // bars
        for (uint8_t i = 0; i < numBar; i++) {
            ModuleDisp::baseDisplay.fillRoundRect(dispMidX - 22, dispMidY + 32 - i * rectDimY, 44, rectDimY - 4, 4, GxEPD_BLACK);
        }

        ModuleDisp::baseDisplay.setCursor(dispMidX - tbw / 2, dispMidY - 45);
        ModuleDisp::baseDisplay.print(cellPercentFormatted);

    } while (ModuleDisp::baseDisplay.nextPage());

    ModuleDisp::isActive = false;
    vTaskDelete(NULL);
}

bool ModuleDisp::isReadyToSleep() {
    return !ModuleDisp::isActive;
}
