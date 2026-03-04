#pragma once

class ExplorIRData
{
public:
    ExplorIRData() { clear(); }

    void clear();

    void print() const;

    int getCO2PPMFiltered() const;
    int getCO2PPMUnfiltered() const;
    float getCO2PercentFiltered() const;
    float getCO2PercentUnfiltered() const;
    int getCO2PPMScaling() const { return co2PPMScaling;}
    int getPressureCompensation() const { return pressureCompensation; }
    int getNitrogenCalibration() const { return calibrationNitrogenSetting; }
    int getAirCalibration() const { return calibrationAirSetting; }
    int getCO2Calibration() const { return calibrationCO2Setting; }

    bool hasSerialNumber() const { return serialNumber[0] != '\0'; }
    bool hasPPMScaling() const { return co2PPMScaling > 0; }
    bool hasPressureCompensation() const { return pressureCompensation >= 0; }
    bool hasCO2Calibration() const { return calibrationCO2Setting >= 0; }
    bool hasNitrogenCalibration() const { return calibrationNitrogenSetting >= 0; }
    bool hasAirCalibration() const { return calibrationAirSetting >= 0; }

    void setCO2Filtered(int newValue) { co2filtered = newValue; }
    void setCO2Unfiltered(int newValue) { co2unfiltered = newValue; }
    void setCO2PPMScaling(int newValue) { co2PPMScaling = newValue; }
    void setPressureCompensation(int newValue) { pressureCompensation = newValue; }
    void setCO2Calibration(int newValue) { calibrationCO2Setting = newValue; }
    void setNitrogenCalibration(int newValue) { calibrationNitrogenSetting = newValue; }
    void setAirCalibration(int newValue) { calibrationAirSetting = newValue; }

    void clearPPMScaling() { co2PPMScaling = -1; }
    void clearPressureCompensation() { pressureCompensation = -1; }
    void clearNitrogenCalibration() { calibrationNitrogenSetting = -1; }
    void clearCO2Calibration() { calibrationCO2Setting = -1; }
    void clearAirCalibration() { calibrationAirSetting = -1;}

    char firmwareDate[16];
    char firmwareTime[16];
    char firmwareVersion[16];
    char serialNumber[32];

private:
    int co2filtered;
    int co2unfiltered;
    int co2PPMScaling;
    int pressureCompensation;
    int calibrationNitrogenSetting;
    int calibrationCO2Setting;
    int calibrationAirSetting;
};
