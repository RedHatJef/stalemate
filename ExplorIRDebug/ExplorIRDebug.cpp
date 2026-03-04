
// main.cpp
#include <iostream>

#include "ExplorIRSensor.h"
#include "SerialPort.h"
#include "SimpleQueue.h"

int main() {
    ExplorIRSensor ir;

    ir.init();
    while (1)
    {
        ir.loop();
        Sleep(1);
    }
}
