//collects data from all sensors

#include <Adafruit_Sensor.h>
//#include "Adafruit_BME680.h"
#include "environment_sensor.h"
#include "position_sensor.h"
#include "ADC.h"
#include "RTC.h"
#include "camera.h"
#include <ArduinoJson.h>
#include "power_measure.h"


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
  if (init_status.bme_){
    pointers.bme_ = get_bme_data(); 
  }
  if (init_status.mpu_){
    pointers.mpu_ = get_mpu_data();
  }
  if (init_status.ads_){
    pointers.ads_ = get_ads_data();
  }
  if(init_status.ina_){
    pointers.ina_ = get_ina_data();
  }
  if (init_status.rtc_){
      pointers.rtc_ = get_rtc();
  }
  return &pointers;
}

void print_sensors_data(pointer_of_sensors * data_, bool motor_state){
  if (init_status.bme_){
    print_data(data_->bme_); 
  } else{
   Serial.println("▲ BME680 (environmental sensor) not found!"); 
   }
  if (init_status.mpu_){
    print_data(data_->mpu_);
  }else{
   Serial.println("▲ MPU**** module (position sensor) not found!"); 
  }
  if (init_status.ads_){
    print_data(data_->ads_);
  }
  else{
    Serial.println("▲ ADS1015 module (analog-to-digital convertor) not found!"); 
  }
  if(init_status.ina_){
    print_data(data_->ina_);
  }else{
    Serial.println("▲ INA3221 module (voltage and current sensor) not detected!");
  }
  if (init_status.rtc_){
    print_data(data_->rtc_);
  } else{
    Serial.println("▲ DS3231 module (real time clock) not found!"); 
  }
}
