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
        case 'S':
        case 's':
        case 'G':
        case 'A':
        case 'a':
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

int ExplorIRSensor::getCompensationFromAltitudeM(int altitudeM)
{
    double pressure = 1013.0 * pow(1.0 - 2.25577e-5 * altitudeM, 5.25588);
    double diff = 1013.0 - pressure;
    double comp = 8192.0 + ((diff * 0.14) / 100.0) * 8192.0;
    int newCompInt = (int)comp;
    EIR_VERBOSE(F("Calculated compensation value from altitudeM ("));
    EIR_VERBOSE(altitudeM);
    EIR_VERBOSE(F(") to ("));
    EIR_VERBOSE(newCompInt);
    EIR_VERBOSELN(F(")"));
    return newCompInt;
}

ExplorIRSensor::ExplorIRSensor()
{
    clearMessageData();
}

ExplorIRSensor::~ExplorIRSensor()
{
    Serial2.end();
}

void ExplorIRSensor::busyWait()
{
    // wait for a response
    update();
    delay(10);
}

void ExplorIRSensor::sendCommand(const char *command) {
    EIR_VERBOSE(F("ExplorIR: Sending command --> "));
    EIR_VERBOSELN(command);
    //Serial2.write("\r\n");
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
    delay(50);
    EIR_VERBOSELN(F("ExplorIR: Stopped polling"));

    data.clear();

    fetchCO2PPMScaling(false);
    delay(50);

    fetchPressureCompensation(false);
    delay(50);

    fetchDigitalFilter(false);
    delay(50);

    currentMValue = -1;
    EIR_VERBOSELN(F("ExplorIR: Requesting Data Field Configuration"));
    sendCommand("M 6\r\n");
    while (currentMValue < 0)
    {
        busyWait();
    }
    EIR_DEBUGLN(F("ExplorIR: Got Data Field Configuration"));

    delay(50);

    EIR_VERBOSELN(F("ExplorIR: Requesting Module Info"));
    sendCommand("Y\r\n");
    while (!data.hasSerialNumber())
    {
        busyWait();
    }
    EIR_DEBUGLN(F("ExplorIR: Got Module Info: "));

    delay(100);

    startPolling();
    EIR_DEBUGLN(F("ExplorIR: Polling re-enabled - ready!"));
}

void ExplorIRSensor::update()
{
    if(!Serial2.available()) return;

    int ic;
    while ((ic = Serial2.read()) > 0)
    {
        char c = (char)ic;
        processRXBuffer(c);
    }
}

void ExplorIRSensor::setAltitudeM(int altitudeM)
{
    stopPolling();

    data.clearPressureCompensation();
    int newValue = getCompensationFromAltitudeM(altitudeM);

    char command[16];
    snprintf(command, sizeof(command), "S %d\r\n", newValue);
    sendCommand(command);
    while (!data.hasPressureCompensation())
    {
        busyWait();
    }

    startPolling();
}

void ExplorIRSensor::setDigitalFilter(int newValue)
{
    EIR_DEBUG(F("Setting digital filter to: "));
    EIR_DEBUGLN(newValue);

    stopPolling();
    data.clearDigitalFilter();

    char command[16];
    snprintf(command, sizeof(command), "A %d\r\n", newValue);
    sendCommand(command);
    while (!data.hasDigitalFilter())
    {
        busyWait();
    }

    startPolling();

    EIR_DEBUG(F("Setting digital filter COMPLETE, new value is "));
    EIR_DEBUGLN(data.getDigitalFilter());
}

int ExplorIRSensor::fetchDigitalFilter(bool togglePolling)
{
    if(togglePolling) stopPolling();

    EIR_VERBOSELN(F("ExplorIR: Requesting Digital filter setting"));

    data.clearDigitalFilter();
    sendCommand("a\r\n");
    while(!data.hasDigitalFilter())
    {
        busyWait();
    }

    EIR_PRINT(F("ExplorIR: Got Digital Filter Value: "));
    EIR_PRINTLN(data.getDigitalFilter());

    if(togglePolling) startPolling();
}


void ExplorIRSensor::calibrateInNitrogenArgon()
{
    EIR_DEBUGLN(F("Zeroing in Nitrogen"));

    stopPolling();

    data.clearNitrogenArgonCalibration();
    sendCommand("U\r\n");
    while (!data.hasNitrogenArgonCalibration())
    {
        busyWait();
    }

    startPolling();

    EIR_DEBUGLN(F("Zeroing in Nitrogen COMPLETE"));
}

void ExplorIRSensor::calibrateIn100PercentCO2()
{
    EIR_DEBUGLN(F("Zeroing in 100% CO2."));

    stopPolling();

    int scaleValue = fetchCO2PPMScaling(false);
    EIR_DEBUG(F("Using CO2 scaling of : "));
    EIR_DEBUGLN(scaleValue);

    int calibrationValue = scaleValue * 100;
    EIR_DEBUG(F("Setting calibration value to : "));
    EIR_DEBUGLN(calibrationValue);

    data.clearCO2Calibration();
    sendCommand("U\r\n");
    while(!data.hasCO2Calibration()) {
        busyWait();
    }

    startPolling();

    EIR_DEBUGLN(F("Zeroing in 100% CO2 COMPLETE."));
}

void ExplorIRSensor::calibrateInFreshAir()
{
    stopPolling();

    data.clearFreshAirCalibration();
    sendCommand("G\r\n");
    while (!data.hasFreshAirCalibration())
    {
        busyWait();
    }

    startPolling();
}

int ExplorIRSensor::fetchCO2PPMScaling(bool togglePolling)
{
    if(togglePolling) stopPolling();

    EIR_VERBOSELN(F("ExplorIR: Requesting PPM Scaling"));
    sendCommand(".\r\n");
    while (!data.hasPPMScaling())
    {
        busyWait();
    }
    EIR_PRINT(F("ExplorIR: Got PPM Scaling: "));
    EIR_PRINTLN(data.getCO2PPMScaling());

    if(togglePolling) startPolling();

    return data.getCO2PPMScaling();
}

int ExplorIRSensor::fetchPressureCompensation(bool togglePolling)
{
    if(togglePolling) stopPolling();

    sendCommand("s\r\n");
    EIR_VERBOSELN(F("ExplorIR: Requesting Pressure Compensation"));
    while (!data.hasPressureCompensation())
    {
        busyWait();
    }

    EIR_PRINT(F("ExplorIR: Got Pressure Compensation: "));
    EIR_PRINTLN(data.getPressureCompensation());

    if(togglePolling) startPolling();

    return data.getPressureCompensation();
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
    sendCommand("K 0\r\n");
    for(int i = 0; i < 10 && currentKValue < 0; i++) {
        busyWait();
    }
}

void ExplorIRSensor::startPolling()
{
    currentKValue = -1;
    sendCommand("K 1\r\n");
    for(int i = 0; i < 10 && currentKValue < 0; i++) {
        busyWait();
    }
}

bool ExplorIRSensor::processRXBuffer(char c)
{
    switch (messageState)
    {
        case MESSAGE_STATE_NONE:
            if (isWhitespace(c))
            {
                // ignore
            }
            else if(c == '?')
            {
                // ignore - it probably thinks we send a blank line and we're just trying to
                // wake it up and reset its state for a new command
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
            data.setNitrogenArgonCalibration(getNumberFromMessage(currentMessageData));
            EIR_VERBOSE_PRINTF1(F("Got new 100%% nitrogen calibration: %d\r\n"), data.getNitrogenArgonCalibration());
            break;
        case 'X':
            data.setCO2Calibration(getNumberFromMessage(currentMessageData));
            EIR_PRINTF1(F("Got new 100%% CO2 calibration: %d\r\n"), data.getCO2Calibration());
            break;
        case 'G':
            data.setFreshAirCalibration(getNumberFromMessage(currentMessageData));
            EIR_PRINTF1(F("Got new fresh air calibration: %d\r\n"), data.getFreshAirCalibration());
            break;
        case 'A':
        case 'a':
            data.setDigitalFilter(getNumberFromMessage(currentMessageData));
            EIR_PRINTF1(F("Got new digital filter: %d\r\n"), data.getDigitalFilter());
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







