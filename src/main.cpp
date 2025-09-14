#include <Arduino.h>
#include <Wire.h>
#include "Display.h"
#include "Devices/Clock.h"
#include "Devices.h"
#include "Devices/SCD40_CO2.h"
#include "Devices/Storage/Storage.h"
#include "Util/Utils.h"
#include "Menu/Items/Main/MainMenu.h"
#include "Menu/UiState.h"
#include "Menu/MenuActionInput.h"
#include "Menu/MenuUtils.h"

static Devices devices;

static Clock clock;
static BMP bmp;
static SHT sht;
static SCD40_CO2 scd40;
static ExplorIRSensor explorIR;

static Display display;
static Storage storage;

void printBootLine(const __FlashStringHelper *s) {
    Serial.println(s);
    display.println(s);
}

void setup() {
    setSystemClock();
    delay(FRAMETIME_MILLIS * 10);
    Serial.begin(115200);
    delay(FRAMETIME_MILLIS);
    for(int i = 0; i < 10; i++) Serial.println();
    Serial.println(F("Booting."));

    devices.clock = &clock;
    devices.bmp = &bmp;
    devices.sht = &sht;
    devices.scd40 = &scd40;
    devices.explorIR = &explorIR;

    Wire.begin();
    Serial.println(F("I2C Data bus ready."));

    display.init(&devices);
    Serial.println(F("Display ready."));

    clock.init();
    clock.update();
    printBootLine(F("Clock OK"));

    // run the bmp a bit so we get a valid altitude we can use with the co2 sensor
    bmp.init();
    for(int i = 0; i < 20; i++) {
        bmp.update();
        delay(FRAMETIME_MILLIS);
    }
    printBootLine(F("BMP390 OK"));

    sht.init();
    sht.update();
    printBootLine(F("SHT31 OK"));

    scd40.init(bmp.getAltitudeM());
    scd40.update();
    printBootLine(F("SCD40 OK"));

    explorIR.init();
    printBootLine(F("ExplorIR CO2 OK"));

    storage.init(&devices);
    printBootLine(F("Storage OK."));

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
    explorIR.update();
    storage.update();

    devices.numSamples = storage.getNumSamples();

    unsigned long endTime = millis();
    unsigned long updateTime = endTime - startTime;
    if(updateTime < FRAMETIME_MILLIS) {
        delay(FRAMETIME_MILLIS - updateTime);
    }

    processSerialInput();
}