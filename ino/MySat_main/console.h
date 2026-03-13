#pragma once
#include "sensors_data.h"
#include "control.h"
#include "data_logger.h"
#include <LittleFS.h>
#define FIRMWARE_VERSION "v.1.3"
#define OUTPUT_FREQUENCE 1500

extern String useWiFi;
extern String ssid;
extern String password;
String callSign = "MYSAT";
extern bool debug_mode_active;
enum TelemetryMode { text,
                     plotter,
                     debug };
TelemetryMode currentMode = text;

enum PlotterMode { environment,
                   position,
                   sunTracker,
                   powerSystem };
PlotterMode plotterMode = environment;

void setWiFi();
void tryConnectWiFi();

void selectPlotterMode() {
  Serial.println("Select plotter mode:");
  Serial.println("1 - Environment (BME680)");
  Serial.println("2 - Position (MPU****)");
  Serial.println("3 - Sun Tracker (ADS1015)");
  Serial.println("4 - Power System (INA3221)");

  while (!Serial.available()) {}
  int selection = Serial.parseInt();

  if (selection >= 1 && selection <= 4) {
    plotterMode = (PlotterMode)(selection - 1);
    Serial.print("Selected mode: ");
    Serial.println(selection);
  } else {
    Serial.println("Invalid selection. Keeping current mode.");
  }
}

void saveCallSign(const String& callSign) {
  File file = LittleFS.open("/callsign.txt", "w");
  if (file) {
    file.println(callSign);
    file.close();
  }
}

void loadCallSign(String& callSign) {
  File file = LittleFS.open("/callsign.txt", "r");
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
void turnConsole() {
  output = !output;
}

void auditFileSystem() {
  Serial.println("\n======= FILE SYSTEM AUDIT (LittleFS) =======");

  size_t total = LittleFS.totalBytes();
  size_t used = LittleFS.usedBytes();
  float usagePercent = (float)used / total * 100.0;

  Serial.printf("TOTAL CAPACITY: %u bytes (%.2f KB)\n", total, total / 1024.0);
  Serial.printf("USED SPACE:     %u bytes (%.2f KB) [%.1f%%]\n", used, used / 1024.0, usagePercent);
  Serial.printf("FREE SPACE:     %u bytes (%.2f KB)\n", total - used, (total - used) / 1024.0);
  Serial.println("--------------------------------------------------");

  File root = LittleFS.open("/");
  if (!root || !root.isDirectory()) {
    Serial.println("Error: Failed to open root directory!");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.printf("[DIR]  /%s\n", file.name());
      int allDirSize = 0;
      
      File subDir = LittleFS.open(file.name());
      File subFile = subDir.openNextFile();
      while (subFile) {
        Serial.printf("      |-- %-20s | %u bytes\n", subFile.name(), subFile.size());
        allDirSize += subFile.size();
        subFile = subDir.openNextFile();
      }
      Serial.printf("Total Directory Size: %u bytes\n", allDirSize);
    } else {
      Serial.printf("[FILE] /%-20s | %u bytes\n", file.name(), file.size());
    }
    file = root.openNextFile();
  }
  Serial.println("==================================================\n");
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
          initServer();
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

      } else if (inputBuffer.equalsIgnoreCase("SwitchTelemetry")) {
        if (currentMode == text) {
          currentMode = plotter;
          reactToCommand("Mode: PLOTTER.");
          selectPlotterMode();
          
        } else if (currentMode == plotter) {
          currentMode = text;
          reactToCommand("Mode: TEXT.");
        }
        recognized = true;

      } else if (inputBuffer.equalsIgnoreCase("SelectPlotterMode")) {
        if (currentMode == plotter) {
          selectPlotterMode();
        } else {
          Serial.println("Switch to plotter mode first (SwitchTelemetry)");
        }
        recognized = true;

      } else if(inputBuffer.equalsIgnoreCase("DebugModeOn")){
        currentMode = debug;
        debug_mode_active = true;
        reactToCommand("Mode: DEBUG.");
        recognized = true;

      } else if(inputBuffer.equalsIgnoreCase("DebugModeOff")){
        debug_mode_active = false;
        currentMode = text;
        reactToCommand("Mode: TEXT.");
        recognized = true;

      }else if(inputBuffer.equalsIgnoreCase("SetRadio")){
        setRadio();
        reactToCommand("Radio(HC-12) AT-config mode activated.");
        recognized = true;
        
      }else if(inputBuffer.equalsIgnoreCase("StartLogging")){
        startLogging();
        reactToCommand("Start logging...");
        recognized = true;
        
      }else if(inputBuffer.equalsIgnoreCase("StopLogging")){
        stopLogging();
        reactToCommand("Stop logging...");
        recognized = true;

      }else if(inputBuffer.equalsIgnoreCase("AuditFileSystem")){
        auditFileSystem();
        pauseToRead();
        recognized = true;

      }else if(inputBuffer.equalsIgnoreCase("ListLogFiles")){
        listLogFiles();
        pauseToRead();
        recognized = true;

      }else if(inputBuffer.equalsIgnoreCase("BlinkLed")){
        startBlink(stateLight);
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

void outputDataText(pointer_of_sensors* data_) {
  Serial.println("\n\n\n" + callSign + "(" FIRMWARE_VERSION ")");

  if (init_status.bme_) {
    if (data_->bme_->data_available) {
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

      Serial.print("  IAQ = ");
      Serial.println(data_->bme_->iaq);

      Serial.print("  IAQ Accuracy: ");
      Serial.println(data_->bme_->iaq_accuracy);

    } else {
      Serial.println("===ENVIRONMENT:==================");
      Serial.println(" IAQ Sensor Warming Up (5 min)...");
    }

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
    Serial.println("▲ MPU**** module (position sensor) not found!");
  }

  if (init_status.ads_) {
    Serial.println("===SUN TRACKER:==================");
    Serial.print("  ph1 (left) = ");
    Serial.print(data_->ads_->ph1);
    Serial.print("   |  ph2 (back) = ");
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

  if(logger.total_rows >= MAX_TOTAL_ROWS && logger.total_rows > 0){
    Serial.println("  Logging: STOPPED (memory full)");
  }else if(logger.enabled){
    Serial.print("  Logging: ACTIVE (period: ");
    Serial.print(logger.period_seconds);
    Serial.print(" s, rows: ");
    Serial.print(logger.total_rows);
    Serial.print("/");
    Serial.print(MAX_TOTAL_ROWS);
    Serial.println(")");
  }

  if (useWiFi.equalsIgnoreCase("Yes")) {
    Serial.println("================================");
    Serial.print("CONNECT VIA WIFI “");
    Serial.print(ssid);
    Serial.println("”:");
    if (LittleFS.exists("/bootstrap.css")) {
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("▲ Files for WebGUI not found!");
    }
    Serial.println("================================");
  } else {
    Serial.println("================================");
    Serial.println("WiFi not configured. Use the command: SetWIFI");
  }
}

void outputDataPlotter(pointer_of_sensors* data_) {
  switch (plotterMode) {
    case environment:
      if (init_status.bme_) {
        Serial.print("Temperature:");
        Serial.print(data_->bme_->temperature);
        Serial.print(',');
        Serial.print("Pressure:");
        Serial.print(data_->bme_->pressure);
        Serial.print(',');
        Serial.print("Humidity:");
        Serial.print(data_->bme_->humidity);
        Serial.print(',');
        Serial.print("GasResistance:");
        Serial.print(data_->bme_->gas_resistance);
      }
      break;

    case position:
      if (init_status.mpu_ && calibration.valid) {
        int roll = round(data_->mpu_->roll - offset_roll);
        int pitch = round(data_->mpu_->pitch - offset_pitch);
        int yaw = round(data_->mpu_->yaw - offset_yaw);

        Serial.print("X:");
        Serial.print(roll);
        Serial.print(',');
        Serial.print("Y:");
        Serial.print(pitch);
        Serial.print(',');
        Serial.print("Z:");
        Serial.print(yaw);
      }
      break;

    case sunTracker:
      if (init_status.ads_) {
        Serial.print("ph1:");
        Serial.print(data_->ads_->ph1);
        Serial.print(',');
        Serial.print("ph2:");
        Serial.print(data_->ads_->ph2);
        Serial.print(',');
        Serial.print("ph3:");
        Serial.print(data_->ads_->ph3);
        Serial.print(',');
        Serial.print("ph4:");
        Serial.print(data_->ads_->ph4);
      }
      break;

    case powerSystem:
      if (init_status.ina_) {
        float IL = data_->ina_->leftSolarPanelCurrent;
        float IR = data_->ina_->rightSolarPanelCurrent;
        if (IL < 0) IL = 0;
        if (IR < 0) IR = 0;

        Serial.print("BattV:");
        Serial.print(data_->ina_->batteryVoltage, 2);
        Serial.print(',');
        Serial.print("BattI:");
        Serial.print(data_->ina_->batteryCurrent, 2);
        Serial.print(',');
        Serial.print("SolarV:");
        Serial.print(data_->ina_->SolarPanelVoltage, 2);
        Serial.print(',');
        Serial.print("SolarI_L:");
        Serial.print(IL, 2);
        Serial.print(',');
        Serial.print("SolarI_R:");
        Serial.print(IR, 2);
      }
      break;
  }
  Serial.println();
}

unsigned long lastOutput = 0;
void outputData(pointer_of_sensors* data_) {
  unsigned long now = millis();

  if (now - lastOutput >= OUTPUT_FREQUENCE && output) {
    if (currentMode == text) {
      outputDataText(data_);
    } else if (currentMode == plotter) {
      outputDataPlotter(data_);
    }
    lastOutput = now;
  }

  handleDataLogging(data_);
}