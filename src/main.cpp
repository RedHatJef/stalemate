#include <Arduino.h>
#include <Wire.h>
#include "Display.h"
#include "Clock.h"
#include "Devices.h"
#include "SCD40_CO2.h"
#include "Storage.h"
#include "Utils.h"
#include "Menu/Items/Main/MainMenu.h"
#include "Menu/UiState.h"
#include "Menu/MenuActionInput.h"
#include "Menu/MenuUtils.h"

static Devices devices;

static Clock clock;
static BMP bmp;
static SHT sht;
static SCD40_CO2 scd40;

static Display display;
static Storage storage;

void setup() {
    setSystemClock();
    delay(FRAMETIME_MILLIS * 10);
    Serial.begin(115200);
    delay(FRAMETIME_MILLIS);
    Serial.println(F("Booting."));

    devices.clock = &clock;
    devices.bmp = &bmp;
    devices.sht = &sht;
    devices.scd40 = &scd40;

    Wire.begin();

    clock.init();
    clock.update();
    Serial.println(F("Clock ready."));

    // run the bmp a bit so we get a valid altitude we can use with the co2 sensor
    bmp.init();
    for(int i = 0; i < 20; i++) {
        bmp.update();
        delay(FRAMETIME_MILLIS);
    }
    Serial.println(F("BMP390 ready."));

    sht.init();
    sht.update();
    Serial.println(F("SHT31 ready."));

    scd40.init(bmp.getAltitudeM());
    scd40.update();
    Serial.println("SCD40 ready.");

    display.init(&devices);
    Serial.println(F("Display ready."));

//    storage.init(&devices);
//    Serial.println(F("Storage ready."));

    setMenuDevices(&devices);
    setMenuStorage(&storage);
    Serial.println(F("Menu System Ready."));
    Serial.println();
    displayMenu();
}

void loop() {
    unsigned long startTime = millis();

    clock.update();
    bmp.update();
    sht.update();
    scd40.update();
    display.update();
//    storage.update();

    unsigned long endTime = millis();
    unsigned long updateTime = endTime - startTime;
    if(updateTime < FRAMETIME_MILLIS) {
        delay(FRAMETIME_MILLIS - updateTime);
    }

    processSerialInput();
}