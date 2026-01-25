#pragma once
#include <LittleFS.h>
#include "sensors_data.h"

const int MAX_TOTAL_ROWS = 3600;     
const int MAX_FILES = 100;          
const unsigned long HOUR_MS = 3600000;
const char* LOGGER_STATE_FILE = "/logger_state.txt";

struct LoggerState {
  bool enabled = false;             
  unsigned long period_seconds = 0;  
  unsigned long last_log_time = 0;   
  unsigned long session_start = 0;  
  int total_rows = 0;             
  int current_file_rows = 0;        
  int file_number = 0;               
  String current_filename = "";     
};

LoggerState logger;

void saveLoggerState() {
  File file = LittleFS.open(LOGGER_STATE_FILE, "w");
  if (file) {
    file.println(logger.enabled ? "1" : "0");
    file.println(logger.period_seconds);
    file.println(logger.total_rows);
    file.println(logger.current_file_rows);
    file.println(logger.current_filename);
    file.println(logger.file_number);  
    file.close();
    
    if (debug_mode_active) {
      Serial.println("[LOGGER] State saved:");
      Serial.println("  Enabled: " + String(logger.enabled));
      Serial.println("  Period: " + String(logger.period_seconds) + "s");
      Serial.println("  Total rows: " + String(logger.total_rows));
      Serial.println("  File number: " + String(logger.file_number));
    }
  }
}

String createCSVHeader() {
  return "RTC_Time,ESP32_Uptime,Temperature,Pressure,Humidity,Gas_Resistance,IAQ,IAQ_Accuracy,"
         "Roll,Pitch,Yaw,PH1,PH2,PH3,PH4,Battery_V,Battery_I,Solar_V,Solar_I_L,Solar_I_R\n";
}

bool createNewLogFile() {
  rtc_struct* rtc = get_rtc();
  
  char filename[64];
  snprintf(filename, sizeof(filename), "/log_%04d%02d%02d_%02d%02d%02d_row%d.csv",
           rtc->year_, rtc->month_, rtc->day_,
           rtc->hour_, rtc->minute_, rtc->second_,
           logger.total_rows);
  
  logger.current_filename = String(filename);
  logger.current_file_rows = 0;
  logger.file_number++;
  
  File file = LittleFS.open(logger.current_filename.c_str(), "w");
  if (!file) {
    Serial.println("[LOGGER] ERROR: Failed to create file!");
    return false;
  }
  
  file.print(createCSVHeader());
  file.close();
  
  logDebug("[LOGGER] New file created: " + logger.current_filename);
  saveLoggerState();
  return true;
}

void loadLoggerState() {
  if (LittleFS.exists(LOGGER_STATE_FILE)) {
    File file = LittleFS.open(LOGGER_STATE_FILE, "r");
    if (file) {
      logger.enabled = (file.readStringUntil('\n').toInt() == 1);
      logger.period_seconds = file.readStringUntil('\n').toInt();
      logger.total_rows = file.readStringUntil('\n').toInt();
      logger.current_file_rows = file.readStringUntil('\n').toInt();
      
      logger.current_filename = file.readStringUntil('\n');
      logger.current_filename.trim();
      
      String file_num_str = file.readStringUntil('\n');
      logger.file_number = file_num_str.toInt();  
      
      file.close();
      
      if (logger.total_rows >= MAX_TOTAL_ROWS) {
        logger.enabled = false;
        saveLoggerState();
        Serial.println("[LOGGER] Limit reached before reboot. Logging disabled.");
        return;
      }
      
      if (logger.enabled && logger.current_filename.length() > 0) {
        if (!LittleFS.exists(logger.current_filename.c_str())) {
          Serial.println("[LOGGER] WARNING: File lost after reboot: " + logger.current_filename);
          Serial.println("[LOGGER] Creating new file...");
          
          if (!createNewLogFile()) {
            Serial.println("[LOGGER] ERROR: Cannot create file!");
            logger.enabled = false;
            saveLoggerState();
            return;
          }
        } else {
          Serial.println("[LOGGER] Resumed after reboot:");
          Serial.println("  Total rows: " + String(logger.total_rows));
          Serial.println("  Current file: " + logger.current_filename);
          Serial.println("  File number: " + String(logger.file_number));
        }
        
        logger.session_start = millis();
        logger.last_log_time = millis();
      }
      
      if (debug_mode_active) {
        Serial.println("[LOGGER] State loaded successfully");
      }
    }
  }
}

void deleteOldestLogFile() {
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  
  String oldest_file = "";
  int smallest_row = 999999;
  
  while (file) {
    String filename = file.name();
    if (filename.startsWith("/log_") && filename.endsWith(".csv")) {
      int row_pos = filename.indexOf("_row");
      if (row_pos > 0) {
        int dot_pos = filename.indexOf(".csv");
        String row_str = filename.substring(row_pos + 4, dot_pos);
        int row_num = row_str.toInt();
        
        if (row_num < smallest_row) {
          smallest_row = row_num;
          oldest_file = filename;
        }
      }
    }
    file = root.openNextFile();
  }
  
  if (oldest_file.length() > 0) {
    LittleFS.remove(oldest_file.c_str());
    logDebug("[LOGGER] Deleted oldest file: " + oldest_file);
    logger.file_number--;
  }
}

void checkHourlyFileRotation() {
  unsigned long current_time = millis();
  unsigned long session_duration = current_time - logger.session_start;
  
  int expected_file = (session_duration / HOUR_MS) + 1;
  
  if (expected_file > logger.file_number) {
    if (logger.file_number >= MAX_FILES) {
      deleteOldestLogFile();
    }
    createNewLogFile();
  }
}

void writeDataRow(pointer_of_sensors* data) {
  if (logger.total_rows >= MAX_TOTAL_ROWS) {
    logger.enabled = false;
    saveLoggerState();
    Serial.println("[LOGGER] Max rows reached");
    return;
  }
  
  checkHourlyFileRotation();
  
  static char csv_line[256];  
  int pos = 0; 
  
  if (init_status.rtc_) {
    pos += snprintf(csv_line + pos, sizeof(csv_line) - pos,
                    "%04d-%02d-%02d %02d:%02d:%02d,",
                    data->rtc_->year_, data->rtc_->month_, data->rtc_->day_,
                    data->rtc_->hour_, data->rtc_->minute_, data->rtc_->second_);
  } else {
    pos += snprintf(csv_line + pos, sizeof(csv_line) - pos, "N/A,");
  }
  
  float uptime = millis() / 1000.0;
  pos += snprintf(csv_line + pos, sizeof(csv_line) - pos, "%.3f,", uptime);
  
  if (init_status.bme_ && data->bme_->data_available) {
    pos += snprintf(csv_line + pos, sizeof(csv_line) - pos,
                    "%.2f,%.2f,%.2f,%.2f,%d,%d,",
                    data->bme_->temperature,
                    data->bme_->pressure,
                    data->bme_->humidity,
                    data->bme_->gas_resistance,
                    (int)data->bme_->iaq,
                    data->bme_->iaq_accuracy);
  } else {
    pos += snprintf(csv_line + pos, sizeof(csv_line) - pos, "N/A,N/A,N/A,N/A,N/A,N/A,");
  }
  
  if (init_status.mpu_ && calibration.valid) {
    pos += snprintf(csv_line + pos, sizeof(csv_line) - pos,
                    "%.1f,%.1f,%.1f,",
                    data->mpu_->roll,
                    data->mpu_->pitch,
                    data->mpu_->yaw);
  } else {
    pos += snprintf(csv_line + pos, sizeof(csv_line) - pos, "N/A,N/A,N/A,");
  }
  
  if (init_status.ads_) {
    pos += snprintf(csv_line + pos, sizeof(csv_line) - pos,
                    "%d,%d,%d,%d,",
                    data->ads_->ph1,
                    data->ads_->ph2,
                    data->ads_->ph3,
                    data->ads_->ph4);
  } else {
    pos += snprintf(csv_line + pos, sizeof(csv_line) - pos, "N/A,N/A,N/A,N/A,");
  }
  
  if (init_status.ina_) {
    float IL = data->ina_->leftSolarPanelCurrent;
    float IR = data->ina_->rightSolarPanelCurrent;
    if (IL < 0) IL = 0;
    if (IR < 0) IR = 0;
    
    pos += snprintf(csv_line + pos, sizeof(csv_line) - pos,
                    "%.2f,%.2f,%.2f,%.2f,%.2f",
                    data->ina_->batteryVoltage,
                    data->ina_->batteryCurrent,
                    data->ina_->SolarPanelVoltage,
                    IL, IR);
  } else {
    pos += snprintf(csv_line + pos, sizeof(csv_line) - pos, "N/A,N/A,N/A,N/A,N/A");
  }
  
  pos += snprintf(csv_line + pos, sizeof(csv_line) - pos, "\n");
  
  File file = LittleFS.open(logger.current_filename.c_str(), "a");
  if (file) {
    file.write((uint8_t*)csv_line, pos);
    file.close();
    
    logger.total_rows++;
    logger.current_file_rows++;

    saveLoggerState();
    
    if (debug_mode_active) {
      Serial.print("[LOGGER] Row written, size: ");
      Serial.print(pos);
      Serial.println(" bytes");
    }
  } else {
    Serial.println("[LOGGER] ERROR: File write failed!");
  }
}

void handleDataLogging(pointer_of_sensors* data) {
  if (!logger.enabled) return;
  
  unsigned long current_time = millis();
  
  if (current_time - logger.last_log_time >= logger.period_seconds * 1000) {
    writeDataRow(data);
    logger.last_log_time = current_time;
  }
}

void startLogging() {
  Serial.println("\n[LOGGER] Starting logging...");
  Serial.print("Enter logging period in seconds (e.g., 1, 5, 10, 60): ");
  
  while (!Serial.available()) {}
  logger.period_seconds = Serial.parseInt();
  
  while (Serial.available()) Serial.read();
  
  if (logger.period_seconds < 1) {
    Serial.println("[LOGGER] Invalid period! Must be >= 1 second.");
    return;
  }
  
  Serial.println(logger.period_seconds);
  
  logger.enabled = true;
  logger.session_start = millis();
  logger.last_log_time = millis();
  logger.total_rows = 0;
  logger.file_number = 0;
  
  if (!createNewLogFile()) {
    logger.enabled = false;
    Serial.println("[LOGGER] Failed to start logging!");
    return;
  }
  
  saveLoggerState();
  
  Serial.println("[LOGGER] Logging STARTED");
  Serial.println("[LOGGER] Period: " + String(logger.period_seconds) + " seconds");
  Serial.println("[LOGGER] Max rows: " + String(MAX_TOTAL_ROWS));
  int max_time_hours = (MAX_TOTAL_ROWS * logger.period_seconds) / 3600;
  Serial.println("[LOGGER] Estimated max time: ~" + String(max_time_hours) + " hours");
  Serial.println("[LOGGER] Files will rotate every hour");
}

void stopLogging() {
  logger.enabled = false;
  saveLoggerState();
  
  Serial.println("\n[LOGGER] Logging STOPPED");
  Serial.println("[LOGGER] Total rows written: " + String(logger.total_rows));
  Serial.println("[LOGGER] Total files created: " + String(logger.file_number));
}

void listLogFiles() {
  Serial.println("\n=== LOG FILES ===");
  
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  
  int count = 0;
  while (file) {
    String filename = file.name();
    if (filename.startsWith("/log_") && filename.endsWith(".csv")) {
      Serial.print(filename);
      Serial.print(" (");
      Serial.print(file.size());
      Serial.println(" bytes)");
      count++;
    }
    file = root.openNextFile();
  }
  
  if (count == 0) {
    Serial.println("No log files found.");
  } else {
    Serial.println("Total files: " + String(count));
  }
  
  Serial.println("=================\n");
}