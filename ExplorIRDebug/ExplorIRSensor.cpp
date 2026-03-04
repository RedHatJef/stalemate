#include "ExplorIRSensor.h"

#include <iostream>

static int getNumberFromMessage(const char* bufPtr)
{
    int result = 0;
    uint8_t numDigits = 0;
    while (numDigits < 5)
    {
        char c = *bufPtr;
        bufPtr++;
        if (c == ' ') continue;
        if (c < '0' || c > '9') break;
        result *= 10;
        result += c - '0';
        numDigits++;
    }
    return result;
}

static bool isWhitespace(char c)
{
    switch (c)
    {
    case ' ':
    case '\0':
    case '\r':
    case '\n':
        return true;
    }

    return false;
}

static bool isFiveDigitResponse(char c)
{
    switch (c)
    {
        case 'Z':
        case 'z':
        case 'V':
        case 'v':
        case 'K':
        case 'M':
        case '.':
        case 's':
        case 'S':
        case 'G':
            return true;
        default:
            return false;
    }
}

static const char* getPollingModeString(int pollingMode)
{
    switch (pollingMode)
    {
        case 0: return "SLEEP";
        case 1: return "STREAMING";
        case 2: return "POLLING";
        default: return "????";
    }
}

static int getCompensationFromAltitudeM(int altitudeM)
{
    double pressure = 1013.0 * pow(1.0 - 2.25577e-5 * altitudeM, 5.25588);
    double diff = 1013.0 - pressure;
    double comp = 8192.0 + ((diff * 0.14) / 100.0) * 8192.0;
    int newCompInt = (int)comp;
    std::cout << "Calculated compensation value from altitudeM (" << altitudeM << ") to (" << newCompInt << ")\n";
    return newCompInt;
}

ExplorIRSensor::ExplorIRSensor()
{
    rxBuf.clear();
    clearMessageData();
}

ExplorIRSensor::~ExplorIRSensor()
{
    rxBuf.clear();
    sp.close();
}

void ExplorIRSensor::init()
{
    if (!sp.open(L"COM6", CBR_9600)) {
        std::cerr << "Failed to open COM5\n";
        return;
    }

    stopPolling();

    data.clear();
    sp.writeString(".\r\n");
    while (!data.hasPPMScaling())
    {
        // wait for a response
        loop();
        Sleep(1);
    }

    sp.writeString("s\r\n");
    while (!data.hasPressureCompensation())
    {
        // wait for a response
        loop();
        Sleep(1);
    }

    currentMValue = -1;
    sp.writeString("M 6\r\n");
    while (currentMValue < 0)
    {
        // wait for a response.
        loop();
        Sleep(1);
    }

    sp.writeString("Y\r\n");
    while (!data.hasSerialNumber())
    {
        // wait for a response
        loop();
        Sleep(1);
    }

    startPolling();
}

void ExplorIRSensor::loop()
{
    int c;
    while ((c = sp.readChar()) > 0)
    {
        if (rxBuf.isFull())
        {
            std::cerr << "RX Buffer is full\n";
            break;
        }
        else
        {
            rxBuf.push((char)c);
        }
    }

    while (processRXBuffer())
    {
        // process everything we have
    }
}

void ExplorIRSensor::setAltitudeM(int altitudeM)
{
    stopPolling();

    data.clearPressureCompensation();
    int newValue = getCompensationFromAltitudeM(altitudeM);

    // TODO: Set to 'S' to make it actually set
    char command[16];
    snprintf(command, sizeof(command), "S %d\r\n", newValue);
    sp.writeString(command);
    while (!data.hasPressureCompensation())
    {
        // wait for a response
        loop();
        Sleep(1);
    }

    startPolling();
}

void ExplorIRSensor::zeroInNitrogen()
{
    std::cout << "Zeroing in Nitrogen\n";

    stopPolling();

    sp.writeString("U\r\n");
    while (!data.hasNitrogenCalibration())
    {
        // wait for a response
        loop();
        Sleep(1);
    }

    startPolling();

    std::cout << "Zeroing in Nitrogen COMPLETE\n";
}

void ExplorIRSensor::zeroIn100PercentCO2()
{
    std::cout << "Zeroing in 100% CO2.\n";

    stopPolling();

    data.clearPPMScaling();
    sp.writeString(".\r\n");
    while (!data.hasPPMScaling())
    {
        loop();
        Sleep(1);
    }

    int calibrationValue = data.getCO2PPMScaling() * 100;
    std::cout << "Setting calibration value to : " << calibrationValue << "\n";

    // sp.writeString("U\r\n");
    // while (!data.hasCO2Calibration())
    // {
    //     // wait for a response
    //     loop();
    //     Sleep(1);
    // }

    startPolling();

    std::cout << "Zeroing in 100% CO2 COMPLETE.\n";
}

void ExplorIRSensor::calibrateInAir()
{
    stopPolling();

    sp.writeString("G\r\n");
    while (!data.hasAirCalibration())
    {
        // wait for a response
        loop();
        Sleep(1);
    }

    startPolling();
}

void ExplorIRSensor::clearMessageData()
{
    currentMessageType = 0;
    memset(currentMessageData, 0, sizeof(currentMessageData));
    currentMessageDataIndex = 0;
}

const char* ExplorIRSensor::getPrintableMessageData()
{
    static const char hexDigits[] = "0123456789ABCDEF";

    memset(printBuf, 0, sizeof(printBuf));
    char* bufPtr = printBuf;
    *bufPtr++ = currentMessageType;
    for (uint8_t i = 0; i < currentMessageDataIndex; i++)
    {
        *bufPtr++ = ' ';
        uint8_t thisInt = currentMessageData[i];
        thisInt &= 0xFF;
        *bufPtr++ = hexDigits[(thisInt >> 4) & 0xF];
        *bufPtr++ = hexDigits[thisInt & 0xF];
    }
    return printBuf;
}

void ExplorIRSensor::stopPolling()
{
    currentKValue = -1;
    sp.writeString("K 0\r\n");
    while (currentKValue < 0)
    {
        // wait for a response.
        loop();
        Sleep(1);
    }
}

void ExplorIRSensor::startPolling()
{
    currentKValue = -1;
    sp.writeString("K 1\r\n");
    while (currentKValue < 0)
    {
        // wait for a response.
        loop();
        Sleep(1);
    }
}

bool ExplorIRSensor::processRXBuffer()
{
    char c;
    if (!rxBuf.pop(c)) return false;

    switch (messageState)
    {
        case MESSAGE_STATE_NONE:
            if (isWhitespace(c))
            {
                // ignore
            }
            else if (c == 'Y')
            {
                numYLines = 0;
                currentMessageType = c;
                messageState = MESSAGE_STATE_GET_DATA;
            }
            else if (isFiveDigitResponse(c))
            {
                currentMessageType = c;
                messageState = MESSAGE_STATE_GET_DATA;
            }
            else
            {
                std::cerr << "Unhandled message header of type (" << c << ")\n";;
                messageState = MESSAGE_STATE_NONE;
                clearMessageData();
            }
            break;

        case MESSAGE_STATE_GET_DATA:
            if (isFiveDigitResponse(currentMessageType))
            {
                currentMessageData[currentMessageDataIndex++] = c;
                if (c == '\r' || c == '\n' || c == '\0')
                {
                    messageState = MESSAGE_STATE_DONE;
                }
                else if (currentMessageDataIndex >= 6)
                {
                    messageState = MESSAGE_STATE_DONE;
                }
            }
            else if (currentMessageType == 'Y')
            {
                currentMessageData[currentMessageDataIndex++] = c;
                if (c == '\r')
                {
                    numYLines++;
                }

                if (numYLines == 2)
                {
                    messageState = MESSAGE_STATE_DONE;
                }
            }
            else
            {
                std::cerr << "Unknown message type to be handled: " << c << "\n";
                messageState = MESSAGE_STATE_DONE;
            }

            break;

        case MESSAGE_STATE_DONE:
            //std::cout << "Got message: " << getPrintableMessageData() << '\n';
            processCompletedMessage();
            clearMessageData();
            //data.print();
            messageState = MESSAGE_STATE_NONE;
            break;
    }

    return true;
}

void ExplorIRSensor::processCompletedMessage()
{
    switch (currentMessageType)
    {
        case 'Z':
            data.setCO2Filtered(getNumberFromMessage(currentMessageData));
            std::cout << "Got new filtered CO2: " << data.getCO2PPMFiltered() << " CO2% = " << data.getCO2PercentFiltered() << "\n";
            break;
        case 'z':
            data.setCO2Unfiltered(getNumberFromMessage(currentMessageData));
            std::cout << "Got new unfiltered CO2: " << data.getCO2PPMUnfiltered() << " CO2% = " << data.getCO2PercentUnfiltered() << "\n";
            break;
        case 'V':
            // filtered temperature value that's not a real temperature, don't save
            std::cout << "Got new filtered temp: " << getNumberFromMessage(currentMessageData) << "\n";
            break;
        case 'v':
            // unfiltered temperature value that's not a real temperature, don't save
            std::cout << "Got new unfiltered temp: " << getNumberFromMessage(currentMessageData) << "\n";
            break;
        case 'K':
            currentKValue = getNumberFromMessage(currentMessageData);
            std::cout << "Got new polling mode: " << getPollingModeString(currentKValue) << "\n";
            break;
        case '.':
            data.setCO2PPMScaling(getNumberFromMessage(currentMessageData));
            std::cout << "Got PPM Scaling: " << data.getCO2PPMScaling() << "\n";
            break;
        case 'M':
            currentMValue = getNumberFromMessage(currentMessageData);
            std::cout << "Got new message format: " << currentMValue << "\n";
            break;
        case 'S':
        case 's':
            data.setPressureCompensation(getNumberFromMessage(currentMessageData));
            std::cout << "Got new pressure compensation: " << currentMValue << "\n";
            break;

        // calibrations
        case 'U':
            data.setNitrogenCalibration(getNumberFromMessage(currentMessageData));
            std::cout << "Got new 100% nitrogen calibration: " << data.getNitrogenCalibration() << "\n";
            break;
        case 'X':
            data.setCO2Calibration(getNumberFromMessage(currentMessageData));
            std::cout << "Got new 100% CO2 calibration: " << data.getCO2Calibration() << "\n";
            break;
        case 'G':
            data.setAirCalibration(getNumberFromMessage(currentMessageData));
            std::cout << "Got new fresh air calibration: " << data.getAirCalibration() << "\n";
            break;

        // device info
        case 'Y':
            {
                char* nextToken = nullptr;
                const char* delims = "\r\n,";
                char* t = nullptr;

                t = strtok_s(currentMessageData, delims, &nextToken);
                if (t == nullptr) break;
                strcat_s(data.firmwareDate, t);

                t = strtok_s(nullptr, delims, &nextToken);
                if (t == nullptr) break;
                strcat_s(data.firmwareTime, t);

                t = strtok_s(nullptr, delims, &nextToken);
                if (t == nullptr) break;
                strcat_s(data.firmwareVersion, t);

                t = strtok_s(nullptr, delims, &nextToken);
                if (t == nullptr) break;
                if (t[0] == ' ') t++; // skip leading space
                strcat_s(data.serialNumber, t);
            }
            std::cout << "Got new sensor info:" <<
                "\n  Date/Time : " << data.firmwareDate << " " << data.firmwareTime <<
                "\n  Version   : " << data.firmwareVersion <<
                "\n  Serial    : " << data.serialNumber <<
                "\n";
            break;

        default:
            std::cerr << "Unhandled message of type: " <<
                currentMessageType << "(" << (int)currentMessageType << ") " <<
                    getPrintableMessageData() << "\n";
    }

}







