//
// Created by redha on 3/10/2025.
//

#include "Display.h"
#include "Adafruit_SSD1306.h"
#include "Fonts/FreeMono9pt7b.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C

static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Display::Display() {
}

void Display::init(const Devices* d) {

    devices = d;

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }

//    display.setFont(&FreeMono9pt7b);
    display.setTextColor(SSD1306_WHITE);

    display.clearDisplay();
    display.display();
}

void Display::update() {
    display.clearDisplay();

    display.setCursor(0, 10);
    display.print(devices->clock->getDateString());
    display.print(' ');
    display.println(devices->clock->getTimeString());

//    display.print("BMP-TempF=");
//    display.println(devices->bmp->getTempF());
//
//    display.print("BMP-AltF=");
//    display.println(devices->bmp->getAltitudeF());
//
//    display.print("SHT-TempF=");
//    display.println(devices->sht->getTempF());
//    display.print("SHT-Hum=");
//    display.print(devices->sht->getHumidity());
//    display.println('%');
//
//    display.print("SCD40-CO2=");
//    display.println(devices->scd40->getCO2PPM());

    display.display();
}
