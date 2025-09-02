//
// Created by redha on 9/2/2025.
//

#include "ExplorIRCO2.h"
#include "Util/SimpleQueue.h"
#include "Util/Utils.h"

ExplorIRCO2::ExplorIRCO2(HardwareSerial &port) : serial(port) {
    rxBuf.clear();
}

void ExplorIRCO2::init() {
    serial.begin(9600);

    // set to polling
    serial.write("K 2\r\n");

    delay(500);
    update();
    update();
    update();
    update();
    update();
    update();

    // get the scale factor
    serial.write(".\r\n");

    delay(500);
    update();
    update();
    update();
    update();
    update();
    update();
    update();

    // get the model info
    serial.write("Y\r\n");

    delay(500);
    update();
    update();
    update();
    update();
    update();
    update();
    update();
}

long lastMillis = 0;

void ExplorIRCO2::update() {
    while(1) {
        int newChar = serial.read();
        if(newChar == -1) break;
        addReceivedChar((char)newChar);
    }

    char message[16];
    if(rxBuf.popMessage(message, sizeof(message))) {
        processMessage(message, sizeof(message));
    }

    if(millis() - lastMillis > 1000) {
        lastMillis = millis();
        serial.write("Z\r\nz\r\n");
    }
}

void ExplorIRCO2::addReceivedChar(char c) {
    if(rxBuf.isFull()) {
        Serial.println(F("ExplorIR: RX buffer is full, dropping data."));
        return;
    }
    rxBuf.push(c);
}

void ExplorIRCO2::processMessage(const char *buf, uint8_t messageSize) {
    if(buf == nullptr) return;

    Serial.print(F("MSG --> "));
    for(int i = 0; i < messageSize; i++) {
        Serial.print((uint8_t)buf[i]);
        Serial.print(' ');
    }
    Serial.println();

    const char* d = &buf[1];

    switch(buf[0]) {
        case 'A':
        case 'a':
            digitalFilter = parseFiveDigits(d);
            break;
        case 'F':
            zeroPointSetting = parseFiveDigits(d);
            break;
        case 'G':
            zeroPointInAir = parseFiveDigits(d);
            break;
        case 'K':
            switch(parseFiveDigits(d)) {
                case 0:
                    currentMode = MODE_COMMAND_SLEEP;
                    break;
                case 1:
                    currentMode = MODE_STREAMING;
                    break;
                case 2:
                    currentMode = MODE_POLLING;
                    break;
                default:
                    Serial.println(F("EXPLORIR: Illegal run mode"));
                    currentMode = MODE_COMMAND_SLEEP;
                    break;
            }
            Serial.print(F("Got Mode: "));
            Serial.println(currentMode);
            break;
        case 'M':
            numMeasurementDataTypes = parseFiveDigits(d);
            break;
        case 'Z':
            filteredCO2PPM = parseFiveDigits(d);
            Serial.print(F("Got Filtered: "));
            Serial.println(getUnfilteredCO2PPM());
            break;
        case 'z':
            unfilteredCO2PPM = parseFiveDigits(d);
            Serial.print(F("Got Unfiltered: "));
            Serial.println(getUnfilteredCO2PPM());
            break;
        case '.':
            ppmScaleFactor = parseFiveDigits(d);
            Serial.print(F("Got Scale Factor: "));
            Serial.println(ppmScaleFactor);
            break;
        default:
            Serial.println(F("Unrecognized message"));
            break;
    }


}
