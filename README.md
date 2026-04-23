<img src="https://drive.google.com/uc?export=download&id=10ScfFZb5kTdQQ1aUS-J6Ik5yzo1wd_EH" alt="MySat" width="150"/>

[MySat Webstore](https://www.mysatkit.com/)

# MySatKit-Firmware
This repository contains the main firmware for MySat Kit microcontrollers (ESP32-CAM and ATmega328P).  <br/><br/>
**Purpose:** Simulate the functionality of a 1U CubeSat nanosatellite.  
**Required software:** Arduino IDE 2.0+.

**Instructions:**
[MYSAT KIT GUIDE](https://docs.google.com/document/d/146EPTvLMzydpwUMsbJWAC3gcRO6yPDe3p8kIpxRUwU4/edit?tab=t.0) [(→ Microcontroller Setup Guide)](https://docs.google.com/document/d/146EPTvLMzydpwUMsbJWAC3gcRO6yPDe3p8kIpxRUwU4/edit?tab=t.mkgezpqxvo88#heading=h.d7als96if9cq)

**Repo structure:**  
- `ino` - contains Arduino projects
  - `MySat_main` - for ESP32-CAM
  - `MySat_Nano_ATmega328p` - for Nano board (ATmega328P)
- `libraries.zip` - archive with libs for ESP32 firmware



---

# Release notes

## V.1.3

**Release date: 06.04.2026**  
**Changes**:
|||
|:-|--|
| New Features → | • implemented local photo storage (circular buffer for last 10 images) |
| | • implemented Telemetry Logging to Flash in CSV format with adjustable periods |
| | • added `BlinkLED` command for quick hardware connection testing |
| | • added persistent storage for logging states (ON/OFF) and WiFi configurations |
| UI Updates → | • new Web GUI Gallery: 10 interactive slots for viewing stored photos |
| | • integrated "Connection Status" indicator to monitor real-time data sync |
| | • revamped "Sunlight Trackers" widget with coordinate axes (X/Y) and sensor mapping |
| | • added "Battery" and "Solar panels" monitors to Web GUI |
| | • added log file download capability from ESP32 directly via Web GUI |
| | • minor changes |
| Bug Fixes → | • minor bug fixes|

**MySat boards support:**
> *full:* v.1.5.6+  
> *partial:* v.1.5.2+

## V.1.2

**Release date:** 2026/04/02
**Changes**:
|||
|:-|--|
| New Features → | • integrated Signal LED system for real-time system status indication |
| | • added Indoor Air Quality (IAQ) index calculation based on the BME680 data |
| | • added option to set a custom callsign for the satellite |
| | • implemented `TurnConsole` command to toggle data broadcasting mode |
| | • implemented LittleFS file system for reliable data storage |
| UI Updates → | • added Arduino IDE Plotter mode for real-time data visualization |
| | • introduced Debug mode for extended system process monitoring |
| | • ensured Web GUI autonomy (offline operation without internet connection) |
| | • minor changes and bug fixes |
| Bug Fixes → | • eliminated latencies and lags in the Web GUI performance |

**MySat boards support:**
> *full:* v.1.5.6+  
> *partial:* v.1.5.2+

## V.1.1  

**Release date:** 2025/10/03   
**Changes**:
|||
|:-|--|
| New Features → | • added support for INA3221, voltage&current sensor|
|| • added rotation angle calculation using the MPUxxxx module|
|| • implemented software protection for servomotor|
| UI Updates → | • new Web GUI widget for the solar navigation system|
|| • displaying photo capture time in Web GUI|
|| • added console commands for controlling servomotor and StarLED |
|| • minor changes and bug fixes|
| Bug Fixes → | • ensured compatibility with v.1.5.5 boards (for HC-12 module)|

**MySat boards support:**
> *full:* v.1.5.3+  
> *partial:* v.1.5.2

## V.1.0.1  

**Release date:** 2025/05/13   
**Changes**:
|||
|:-|--|
| New Features → |• added Wi-Fi configuration function |
|| • added RTC configuration function|
| Bug Fixes → | • minor bug fixes|

**MySat boards support:**
> *full:* v.1.5.2 - v.1.5.4 (no newer!)

## V.1.0.0

**Release date:** 2025/01/18  
**Changes**:  
[ initial release ]

**MySat boards support:**
> *full:* v.1.5.2 - v.1.5.4 (no newer!)
