<img src="https://drive.google.com/uc?export=download&id=10ScfFZb5kTdQQ1aUS-J6Ik5yzo1wd_EH" alt="MySat" width="150"/>

# MySatKit-Firmware
This repository contains the main firmware for MySat Kit microcontrollers (ESP32-CAM and ATmega328P).  <br/><br/>
**Purpose:** To simulate the functionality of a 1U CubeSat nanosatellite.  
**Required software:** Arduino IDE 2.0+.

**Instructions:**
[MYSAT KIT GUIDE](https://docs.google.com/document/d/146EPTvLMzydpwUMsbJWAC3gcRO6yPDe3p8kIpxRUwU4/edit?tab=t.0) [(→ Microcontroller Setup Guide)](URL)

**Repo structure:**  
- `ino` - contains Arduino projects
  - `MySat_main` - for ESP32-CAM
  - `MySat_Nano_ATmega328p` - for Nano board (ATmega328P)
- `libraries.zip` - archive with libs for ESP32 firmware



---

# Release notes

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
<br/><br/>

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
<br/><br/>

## V.1.0.0

**Release date:** 2025/01/18  
**Changes**:  
[ initial release ]

**MySat boards support:**
> *full:* v.1.5.2 - v.1.5.4 (no newer!)
