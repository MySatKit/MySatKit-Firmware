#pragma once
#include "sensors_data.h"
#include "control.h"
#include <SPIFFS.h>
#define FIRMWARE_VERSION "v.1.2"
#define OUTPUT_FREQUENCE 1500

extern String useWiFi;
extern String ssid;
extern String password;
String callSign = "MYSAT";

void setWiFi();
void tryConnectWiFi();

void saveCallSign(const String& callSign) {
  if (!SPIFFS.begin(true)) return;

  File file = SPIFFS.open("/callsign.txt", "w");
  if (file) {
    file.println(callSign);
    file.close();
  }
}

void loadCallSign(String& callSign) {
  if (!SPIFFS.begin(true)) return;

  File file = SPIFFS.open("/callsign.txt", "r");
  if (file) {
    callSign = file.readStringUntil('\n');
    callSign.trim();
    file.close();
  }
}

void setCallSign() {
  Serial.print("Enter the Satellite call sign: ");
  while (!Serial.available()) {}
  callSign = Serial.readStringUntil('\n');
  callSign.trim();
  Serial.println(callSign);
  saveCallSign(callSign);

  if (callSign.length() == 0) {
    callSign = "MYSAT";
  }
}

void reactToCommand(String str) {
  Serial.println("\n\n  ▓");
  Serial.println("  ▓" + str);
  Serial.println("  ▓\n\n");
  delay(2000);
}

bool output = true;
unsigned long timeStop = 0;
void turnConsole() {
  if (output) {
    output = false;
    timeStop = millis();
  }
}

void checkConsoleTimer() {
  if (!output && millis() - timeStop >= 3000) {
    output = true;
  }
}

void handleCommands() {  // read commands for changing data
  static String inputBuffer = "";

  while (Serial.available() > 0) {
    char inChar = Serial.read();
    if (inChar == '\r') continue;
    if (inChar == '\n') {
      inputBuffer.trim();

      bool recognized = false;

      if (inputBuffer.equalsIgnoreCase("ChangeTime")) {
        readUARTTime();
        recognized = true;

      } else if (inputBuffer.equalsIgnoreCase("SetWIFI")) {
        setWiFi();
        if (useWiFi.equalsIgnoreCase("Yes")) {
          tryConnectWiFi();
        }
        recognized = true;

      } else if (inputBuffer.equalsIgnoreCase("TurnLed")) {
        light_on();
        recognized = true;

      } else if (inputBuffer.equalsIgnoreCase("SolarDeploy")) {
        setStateMotor(true);
        reactToCommand("Deploying Solar Panels...");
        recognized = true;

      } else if (inputBuffer.equalsIgnoreCase("SolarRetract")) {
        setStateMotor(false);
        reactToCommand("Retracting Solar Panels...");
        recognized = true;

      } else if (inputBuffer.equalsIgnoreCase("SolarMove")) {
        setStateMotor(!stateMotor);
        reactToCommand("Moving Solar Panels...");
        recognized = true;

      } else if (inputBuffer.equalsIgnoreCase("Calibrate")) {
        calibrateMPU();
        pauseToRead();
        recognized = true;

      } else if (inputBuffer.equalsIgnoreCase("TurnConsole")) {
        reactToCommand("Turning Console...");
        turnConsole();
        recognized = true;

      } else if (inputBuffer.equalsIgnoreCase("SetCallSign")) {
        setCallSign();
        loadCallSign(callSign);
        recognized = true;
      }

      if (inputBuffer.length() >= 3 && !recognized) {
        reactToCommand("! Command unrecognized.");
      }

      inputBuffer = "";
    } else {
      inputBuffer += inChar;
    }
  }
}

unsigned long lastOutput = 0;
void outputData(pointer_of_sensors* data_) {
  unsigned long now = millis();
  checkConsoleTimer();

  if (now - lastOutput >= OUTPUT_FREQUENCE && output) {
    Serial.println("\n\n\n" + callSign + "(" FIRMWARE_VERSION ")");

    if (init_status.bme_) {
      Serial.println("===ENVIRONMENT:==================");
      Serial.print("  Temperature = ");
      Serial.print(data_->bme_->temperature);
      Serial.println(F(" *C"));

      Serial.print("  Pressure = ");
      Serial.print(data_->bme_->pressure);
      Serial.println(" hPa");

      Serial.print("  Humidity = ");
      Serial.print(data_->bme_->humidity);
      Serial.println(" %");

      Serial.print("  Gas resistance = ");
      Serial.print(data_->bme_->gas_resistance);
      Serial.println(" KOhms");
    } else {
      Serial.println("▲ BME680 (environmental sensor) not found!");
    }

    if (init_status.mpu_) {
      Serial.println("===POSITION:=====================");
      if (!calibration.valid) {
        Serial.println("MPU**** is not calibrated.");
      } else {

        int roll = round(data_->mpu_->roll - offset_roll);
        int pitch = round(data_->mpu_->pitch - offset_pitch);
        int yaw = round(data_->mpu_->yaw - offset_yaw);

        Serial.printf("  x = %s%d   deg\n", (roll > 0 ? "+" : ""), roll);
        Serial.printf("  y = %s%d   deg\n", (pitch > 0 ? "+" : ""), pitch);
        Serial.printf("  z = %s%d   deg\n", (yaw > 0 ? "+" : ""), yaw);
      }
    } else {
      Serial.println("▲ MPU9250 module (position sensor) not found!");
    }

    if (init_status.ads_) {
      Serial.println("===SUN TRACKER:==================");
      Serial.print("  ph1 ( left) = ");
      Serial.print(data_->ads_->ph1);
      Serial.print("  |  ph2 ( back) = ");
      Serial.println(data_->ads_->ph2);

      Serial.print("  ph3 (right) = ");
      Serial.print(data_->ads_->ph3);
      Serial.print("  |  ph4 (front) = ");
      Serial.println(data_->ads_->ph4);
    } else {
      Serial.println("▲ ADS1015 module (analog-to-digital convertor) not found!");
    }

    if (init_status.ina_) {
      float IL = data_->ina_->leftSolarPanelCurrent;
      float IR = data_->ina_->rightSolarPanelCurrent;
      if (IL < 0) IL = 0;
      if (IR < 0) IR = 0;

      Serial.println("===POWER SYSTEM:==================");
      Serial.print("  Battery:      U = ");
      Serial.print(data_->ina_->batteryVoltage, 2);
      Serial.print("  V,  I =  ");
      Serial.print(data_->ina_->batteryCurrent, 2);
      Serial.println(" mA");

      Serial.print("  Solar Panels: U = ");
      Serial.print(data_->ina_->SolarPanelVoltage, 2);
      Serial.print("  V,");

      if (IL + IR > 5) {
        Serial.print("  I_L = ");
        Serial.print(IL, 2);
        Serial.print(" mA,  I_R = ");
        Serial.print(IR, 2);
        Serial.println(" mA");
      } else {
        Serial.println("  I =  low ");
      }
    } else {
      Serial.println("▲ INA3221 module (triple-channel current and voltage sensore) not found!");
    }

    if (init_status.rtc_) {
      Serial.println("================================");
      Serial.print("  Time: ");
      Serial.print(data_->rtc_->year_);
      Serial.print(".");
      if (data_->rtc_->month_ < 10) {
        Serial.print(0);
        Serial.print(data_->rtc_->month_);
      } else {
        Serial.print(data_->rtc_->month_);
      }
      Serial.print(".");
      if (data_->rtc_->day_ < 10) {
        Serial.print(0);
        Serial.print(data_->rtc_->day_);
      } else {
        Serial.print(data_->rtc_->day_);
      }
      Serial.print("  ");
      if (data_->rtc_->hour_ < 10) {
        Serial.print(0);
        Serial.print(data_->rtc_->hour_);
      } else {
        Serial.print(data_->rtc_->hour_);
      }
      Serial.print(":");
      if (data_->rtc_->minute_ < 10) {
        Serial.print(0);
        Serial.print(data_->rtc_->minute_);
      } else {
        Serial.print(data_->rtc_->minute_);
      }
      Serial.print(":");
      if (data_->rtc_->second_ < 10) {
        Serial.print(0);
        Serial.println(data_->rtc_->second_);
      } else {
        Serial.println(data_->rtc_->second_);
      }
    } else {
      Serial.println("▲ DS3231 module (real time clock) not found!");
    }

    Serial.print(stateMotor ? "  Solar panels: Deployed" : "  Solar panels: Retracted");
    Serial.print("  |  ");
    Serial.print("StarLED: ");
    Serial.println(stateLight ? "ON" : "OFF");

    if (useWiFi.equalsIgnoreCase("Yes")) {
      server.handleClient();
      Serial.println("================================");
      Serial.print("CONNECT VIA WIFI “");
      Serial.print(ssid);
      Serial.println("”:");
      Serial.println(WiFi.localIP());
      Serial.println("================================");
    } else {
      Serial.println("================================");
      Serial.println("WiFi not configured. Use the command: SetWIFI");
    }
    lastOutput = now;
  }
}