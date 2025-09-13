#include "ExplorIRSensor.h"
#include "Util/Utils.h"

#define EIR_DEBUGGING
#undef  EIR_VERBOSE_DEBUGGING

#define EIR_PRINT(x)                    Serial.print(x)
#define EIR_PRINTLN(x)                  Serial.println(x)
#define EIR_PRINTF1(x, y)               Serial.printf(x, y)
#define EIR_PRINTF2(x, y, z)            Serial.printf(x, y, z)
#define EIR_PRINTF3(x, y, z, w)         Serial.printf(x, y, z, w)

#ifdef EIR_VERBOSE_DEBUGGING
#define EIR_VERBOSELN(x)                EIR_PRINTLN(x)
#define EIR_VERBOSE(x)                  EIR_PRINT(x)
#define EIR_VERBOSE_PRINTF1(x, y)       EIR_PRINTF1(x, y)
#define EIR_VERBOSE_PRINTF2(x, y, z)    EIR_PRINTF2(x, y, z)
#define EIR_VERBOSE_PRINTF3(x, y, z, w) EIR_PRINTF3(x, y, z, w)
#else
#define EIR_VERBOSELN(x)
#define EIR_VERBOSE(x)
#define EIR_VERBOSE_PRINTF1(x, y)
#define EIR_VERBOSE_PRINTF2(x, y, z)
#define EIR_VERBOSE_PRINTF3(x, y, z, w)
#endif

#ifdef EIR_DEBUGGING
#define EIR_DEBUGLN(x)                  EIR_PRINTLN(x)
#define EIR_DEBUG(x)                    EIR_PRINT(x)
#define EIR_DEBUG_PRINTF1(x, y)         EIR_PRINTF1(x, y)
#define EIR_DEBUG_PRINTF2(x, y, z)      EIR_PRINTF2(x, y, z)
#define EIR_DEBUG_PRINTF3(x, y, z, w)   EIR_PRINTF3(x, y, z, w)
#else
#define EIR_DEBUGLN(x)
#define EIR_DEBUG(x)
#define EIR_DEBUG_PRINTF1(x, y)
#define EIR_DEBUG_PRINTF2(x, y, z)
#define EIR_DEBUG_PRINTF3(x, y, z, w)
#endif

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
    EIR_PRINT(F("Calculated compensation value from altitudeM ("));
    EIR_PRINT(altitudeM);
    EIR_PRINT(F(") to ("));
    EIR_PRINT(newCompInt);
    EIR_PRINTLN(F(")"));
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
    Serial2.end();
}

void ExplorIRSensor::busyWait()
{
    // wait for a response
    update();
    delay(100);
}

void ExplorIRSensor::sendCommand(const char *command) {
    EIR_VERBOSE(F("ExplorIR: Sending command --> "));
    EIR_VERBOSELN(command);
    Serial2.write(command);
    Serial2.flush();
}

void ExplorIRSensor::init()
{
    EIR_DEBUGLN(F("ExplorIR: Starting"));

    Serial2.begin(9600);
    delay(FRAMETIME_MILLIS);
    EIR_DEBUGLN(F("ExplorIR: Serial port started"));

    stopPolling();
    EIR_VERBOSELN(F("ExplorIR: Stopped polling"));

    data.clear();
    EIR_VERBOSELN(F("ExplorIR: Requesting PPM Scaling"));
    sendCommand(".\r\n");
    while (!data.hasPPMScaling())
    {
        busyWait();
    }
    EIR_DEBUGLN(F("ExplorIR: Got PPM Scaling"));

    delay(100);

    sendCommand("s\r\n");
    EIR_VERBOSELN(F("ExplorIR: Requesting Pressure Compensation"));
    while (!data.hasPressureCompensation())
    {
        busyWait();
    }
    EIR_DEBUGLN(F("ExplorIR: Got Pressure Compensation"));

    delay(100);

    currentMValue = -1;
    EIR_VERBOSELN(F("ExplorIR: Requesting Data Field Configuration"));
    sendCommand("M 6\r\n");
    while (currentMValue < 0)
    {
        busyWait();
    }
    EIR_DEBUGLN(F("ExplorIR: Got Data Field Configuration"));

    delay(100);

    EIR_VERBOSELN(F("ExplorIR: Requesting Module Info"));
    sendCommand("Y\r\n");
    while (!data.hasSerialNumber())
    {
        busyWait();
    }
    EIR_DEBUGLN(F("ExplorIR: Got Module Info"));

    delay(100);

    startPolling();
    EIR_DEBUGLN(F("ExplorIR: Polling re-enabled - ready!"));
}

void ExplorIRSensor::update()
{
    if(!Serial2.available()) return;

    int ic;
    char c;

    while ((ic = Serial2.read()) > 0)
    {
        c = (char)ic;
        EIR_VERBOSE(F("EIRC: "));
        EIR_VERBOSELN(ic);
        if (rxBuf.isFull())
        {
            EIR_PRINTLN(F("RX Buffer is full"));
            Serial.println();
            rxBuf.printStats(true);
            Serial.println();

            while(1) delay(100);
            break;
        }
        else
        {
            rxBuf.push(c);
            #ifdef EIR_VERBOSE_DEBUGGING
            rxBuf.printStats();
            #endif
        }

        while (processRXBuffer())
        {
            // process everything we have
        }
    }

    if(rxBuf.isEmpty()) {
        EIR_VERBOSELN(F("Clearing rxBuf"));
        rxBuf.clear();
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
    sendCommand(command);
    while (!data.hasPressureCompensation())
    {
        busyWait();
    }

    startPolling();
}

void ExplorIRSensor::zeroInNitrogen()
{
    EIR_DEBUGLN(F("Zeroing in Nitrogen"));

    stopPolling();

    sendCommand("U\r\n");
    while (!data.hasNitrogenCalibration())
    {
        busyWait();
    }

    startPolling();

    EIR_DEBUGLN(F("Zeroing in Nitrogen COMPLETE"));
}

void ExplorIRSensor::zeroIn100PercentCO2()
{
    EIR_DEBUGLN(F("Zeroing in 100% CO2."));

    stopPolling();

    data.clearPPMScaling();
    sendCommand(".\r\n");
    while (!data.hasPPMScaling())
    {
        busyWait();
    }

    int calibrationValue = data.getCO2PPMScaling() * 100;
    EIR_DEBUG(F("Setting calibration value to : "));
    EIR_DEBUGLN(calibrationValue);

    // sp.writeString("U\r\n");
    // while (!data.hasCO2Calibration())
    // {
    //     // wait for a response
    //     update();
    //     Sleep(1);
    // }

    startPolling();

    EIR_DEBUGLN(F("Zeroing in 100% CO2 COMPLETE."));
}

void ExplorIRSensor::calibrateInAir()
{
    stopPolling();

    sendCommand("G\r\n");
    while (!data.hasAirCalibration())
    {
        busyWait();
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
    while (currentKValue < 0)
    {
        sendCommand("K 0\r\n");
        for(int i = 0; i < 10; i++) {
            busyWait();
        }
    }
}

void ExplorIRSensor::startPolling()
{
    currentKValue = -1;
    while (currentKValue < 0)
    {
        sendCommand("K 1\r\n");
        for(int i = 0; i < 10; i++) {
            busyWait();
        }
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
                EIR_PRINTF2(F("Unhandled message header of type (%c = %d)\r\n"), c, (int)c);
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
                EIR_PRINT(F("Unknown message type to be handled: "));
                EIR_PRINTLN((int)c);
                messageState = MESSAGE_STATE_DONE;
            }

            break;

        case MESSAGE_STATE_DONE:
            //Serial.printf(F("Got message: %s\n"), getPrintableMessageData());
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
            EIR_VERBOSE_PRINTF2(F("Got new filtered CO2  : %d (CO2%% = %3.2f%%)\r\n"), data.getCO2PPMFiltered(), data.getCO2PercentFiltered());
            break;
        case 'z':
            data.setCO2Unfiltered(getNumberFromMessage(currentMessageData));
            EIR_VERBOSE_PRINTF2(F("Got new unfiltered CO2: %d (CO2%% = %3.2f%%)\r\n"), data.getCO2PPMUnfiltered(), data.getCO2PercentUnfiltered());
            break;
        case 'V':
            // filtered temperature value that's not a real temperature, don't save
            EIR_VERBOSE_PRINTF1(F("Got new filtered temp: %d\r\n"), getNumberFromMessage(currentMessageData));
            break;
        case 'v':
            // unfiltered temperature value that's not a real temperature, don't save
            EIR_VERBOSE_PRINTF1(F("Got new unfiltered temp: %d\r\n"), getNumberFromMessage(currentMessageData));
            break;
        case 'K':
            currentKValue = getNumberFromMessage(currentMessageData);
            EIR_VERBOSE_PRINTF1(F("Got new polling mode: %s\r\n"), getPollingModeString(currentKValue));
            break;
        case '.':
            data.setCO2PPMScaling(getNumberFromMessage(currentMessageData));
            EIR_VERBOSE_PRINTF1(F("Got new PPM Scaling: %d\r\n"), data.getCO2PPMScaling());
            break;
        case 'M':
            currentMValue = getNumberFromMessage(currentMessageData);
            EIR_VERBOSE_PRINTF1(F("Got new message format: %d\r\n"), currentMValue);
            break;
        case 'S':
        case 's':
            data.setPressureCompensation(getNumberFromMessage(currentMessageData));
            EIR_VERBOSE_PRINTF1(F("Got new pressure compensation: %d\r\n"), data.getPressureCompensation());
            break;

        // calibrations
        case 'U':
            data.setNitrogenCalibration(getNumberFromMessage(currentMessageData));
            EIR_VERBOSE_PRINTF1(F("Got new 100%% nitrogen calibration: %d\r\n"), data.getNitrogenCalibration());
            break;
        case 'X':
            data.setCO2Calibration(getNumberFromMessage(currentMessageData));
            EIR_PRINTF1(F("Got new 100%% CO2 calibration: %d\r\n"), data.getCO2Calibration());
            break;
        case 'G':
            data.setAirCalibration(getNumberFromMessage(currentMessageData));
            EIR_PRINTF1(F("Got new fresh air calibration: %d\r\n"), data.getAirCalibration());
            break;

        // device info
        case 'Y':
            {
                const char* delims = "\r\n,";
                char* t = nullptr;

                data.clearModuleInfo();

                t = strtok(currentMessageData, delims);
                if (t == nullptr) break;
                strcpy(data.firmwareDate, t);

                t = strtok(nullptr, delims);
                if (t == nullptr) break;
                strcpy(data.firmwareTime, t);

                t = strtok(nullptr, delims);
                if (t == nullptr) break;
                strcpy(data.firmwareVersion, t);

                t = strtok(nullptr, delims);
                if (t == nullptr) break;

                if (t[0] == ' ') t++; // skip leading space
                strcpy(data.serialNumber, t);
            }

            Serial.print(F("Got NEW "));
            data.printModuleInfo();

            break;

        default:
            EIR_PRINTF3(F("Unhandled message of type: (%c = %d) --> %s\r\n"), currentMessageType, (int)currentMessageType, getPrintableMessageData());
            break;
    }

}







