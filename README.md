
<p align="center">
    <img src="img/Title.jpg" alt="STALEMATE" width="80%">
</p>

<p align="center">
    <img src="https://img.shields.io/badge/MCU-AVR128DA32-blue" />
    <img src="https://img.shields.io/badge/Framework-Arduino-00979D?logo=arduino&logoColor=white" />
    <img src="https://img.shields.io/badge/Platform-PlatformIO-ff6600?logo=platformio&logoColor=white" />
    <img src="https://img.shields.io/badge/status-COMPLETE-22783C" />
    <br>
    <img src="https://img.shields.io/badge/Debug-UPDI-B9BEC3" />
    <img src="https://img.shields.io/badge/Bus-I2C-c66a00" />
    <img src="https://img.shields.io/badge/Bus-SPI-c66a00" />
    <img src="https://img.shields.io/badge/Bus-UART-c66a00" />
    <img src="https://img.shields.io/badge/Storage-SD%20Card-9B91AA" />
    <img src="https://img.shields.io/badge/Storage-EEPROM-9B91AA" />
    <br>
    <img src="https://img.shields.io/badge/RTC-DS3231-3a9d45" />
    <img src="https://img.shields.io/badge/CO₂-SCD41-3399ff" />
    <img src="https://img.shields.io/badge/CO₂-ExplorIR--M--100-3399ff" />
    <img src="https://img.shields.io/badge/RH%2FTemp-SHT31-c44536" />
    <img src="https://img.shields.io/badge/Pressure-BMP390-b8860b" />
</p>

---

<h1>STALEMATE</h1>

# Project Overview

<p align="center"><img src="img/Corner_CloseUp.jpg" width="85%"></p>

Many of the underground caves in Texas can be difficult to explore, either at all times or just during summer months, due to high CO2 levels.  These levels can range from atmospheric to 1-2% (noticable) all the way up to 3-5% (dangerous).  High CO2 levels can be dangerous and at times life threatening.  Gaining an understanding of the cave atmosphere over time can help to identify better times for exploration and study of the cave, and can quantify the impact higher CO2 levels have on cave fauna.

The goal of this project is to build a long-term data logging system that can monitor typical gasses and conditions found in a cave and record them to storage for later analysis.

# Architecture
## Parts Table
| Part Id | Part Name | Datasheet | Breakout Purchase Link |
|-----------|---------|-----------|------------------------|


## Core Processor Selection
## Storage
## RTC
## SHT31
## BMP380
## CO2
### SCD41
### Explor-IR-M-100

# Construction
## Electronics

## Carrier/Enclosure

It's hard to under-emphasize the abuse some of the cave devices I make go through.  They're thrown in cave packs, dragged through mud, dropped down shafts, jostled next to nalgene bottles, covered in mud, and otherwise unpolitely handled.  Any electronics made for this system needed to account for typical operator usages, so a PCB carrier was needed.  This carrier would be glued into the enclosure, and make it easy to remove the PCB for debugging/programming or other modifications.

The carrier was designed and rendered in Autodesk Fusion360.

<p align="center">
<img src="img/PCB_Carrier_Model_Assembled.jpg" width="60%">
</p>
<p align="center">
<img src="img/PCB_Carrier_Model_Exploded.jpg" width="60%">
</p>

## Calibration Box

The most important functionality of this device is its ability to detect up to 100% co2.  A calibration box was built to enclose the sensor, so it could be flooded with 100% CO2 (from a tank) as well as 100% Argon (to verify 0% CO2)

<p align="center"><img src="img/CalibrationChamber.jpg" width="60%"></p>

## Working out the bugs
<p align="center"><img src="img/PrototypeOnMessyDesk.jpg" width="60%"></p>
<p align="center"><img src="img/EnclosureMockUp.jpg" width="60%"></p>
<p align="center"><img src="img/StalemateInAction.gif"></p>

# Final Result
The Stalemate was taken into several caves in central texas to gather data.  

<p align="center"><img src="img/StaleMate_In_Cave.jpg" width="85%"></p>
<p align="center"><img src="img/Overhead_WaterDroplets.jpg" width="85%"></p>

## Sample Data


# Work Left / Known Bugs
* Unit will not start up without SD card inserted
  * The unit should start up and the display should run, even if nothing can be saved to the SD card.
* Explor-IR sensor
  * Sensor drifts over time and loses calibration
  * Reach out to manufacturer for guidance
* O2 Sensor
  * Add galvanic O2 sensor to measure in-cave oxygen

