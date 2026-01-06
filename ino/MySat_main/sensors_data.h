//collects data from all sensors
#pragma once

#include <Adafruit_Sensor.h>
//#include "Adafruit_BME680.h"
#include "environment_sensor.h"
#include "position_sensor.h"
#include "ADC.h"
#include "RTC.h"
#include "camera.h"
#include <ArduinoJson.h>
#include "power_measure.h"

extern bool debug_mode_active;

void logDebug(String message){
  if(debug_mode_active){
    Serial.print("[");
    Serial.print(millis());
    Serial.print(" ms] ");
    Serial.println(message);
  }
}

struct pointer_of_sensors{
  bme_struct * bme_;
  mpu * mpu_;
  ads_struct * ads_;
  ina_struct * ina_;
  rtc_struct * rtc_;
} pointers;

struct init_status_sensors{
  bool bme_;
  bool mpu_;
  bool ads_;
  bool ina_;
  bool rtc_;
  bool camera_;
} init_status;

void initSensors(){
  init_status.bme_ = initBME();
  init_status.mpu_ = initMPU();
  init_status.ads_ = initADS();
  init_status.ina_ = initINA();
  init_status.rtc_ = initRTC();
  init_camera();
}

pointer_of_sensors * get_sensors_data(){
  logDebug("---Reading Sensors START---");
  if (init_status.bme_){
    unsigned long t = millis();
    pointers.bme_ = get_bme_data(); 
    unsigned long dt = millis() - t;
    if (debug_mode_active) logDebug("BME680 Read: " + String(dt) + " ms");
  }
  if (init_status.mpu_){
    unsigned long t = millis();
    pointers.mpu_ = get_mpu_data();
    unsigned long dt = millis() - t;
    if (debug_mode_active) logDebug("MPU**** Read: " + String(dt) + " ms");
  }
  if (init_status.ads_){
    unsigned long t = millis();
    pointers.ads_ = get_ads_data();
    unsigned long dt = millis() - t;
    if (debug_mode_active) logDebug("ADS1015 Read: " + String(dt) + " ms");
  }
  if(init_status.ina_){
    unsigned long t = millis();
    pointers.ina_ = get_ina_data();
    unsigned long dt = millis() - t;
    if (debug_mode_active) logDebug("INA3221 Read: " + String(dt) + " ms");
  }
  if (init_status.rtc_){
      pointers.rtc_ = get_rtc();
  }
  logDebug("---Reading Sensors END---");
  return &pointers;
}
