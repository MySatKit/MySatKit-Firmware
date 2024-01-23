#include <Adafruit_Sensor.h>
//#include "Adafruit_BME680.h"
#include "bme.h"
#include "mpu.h"
#include "ads.h"
#include "rtc.h"
#include "camera.h"
#include <ArduinoJson.h>

String json_string = "";

const size_t bufferSize = JSON_OBJECT_SIZE(23);

DynamicJsonDocument json_sensors(bufferSize);



struct pointer_of_sensors{
  bme_struct * bme_;
  mpu * mpu_;
  ads_struct * ads_;
  rtc_struct * rtc_;
} pointers;

struct init_status_sensors{
  bool bme_;
  bool mpu_;
  bool ads_;
  bool rtc_;
  bool camera_;
} init_status;

void initSensors(){
  init_status.bme_ = initBME();
  init_status.mpu_ = initMPU();
  init_status.ads_ = initADS();
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
  if (init_status.rtc_){
      pointers.rtc_ = get_rtc();
  }
  return &pointers;
}

String * get_all_sensor_data(pointer_of_sensors * data_){
  json_string = "";
  if (init_status.ads_){
    json_sensors["ph1"] = data_->ads_->ph1;
    json_sensors["ph2"] = data_->ads_->ph2;
    json_sensors["ph3"] = data_->ads_->ph3;
    json_sensors["ph4"] = data_->ads_->ph4;
  } else{
    json_sensors["ph1"] = 0;
    json_sensors["ph2"] = 0;
    json_sensors["ph3"] = 0;
    json_sensors["ph4"] = 0;
  }
  if (init_status.bme_){
    json_sensors["temperature"] = data_->bme_->temperature;
    json_sensors["humidity"] = data_->bme_->humidity;
    json_sensors["gas_resistance"] = data_->bme_->gas_resistance;
    json_sensors["pressure"] = data_->bme_->pressure;
  } else{
    json_sensors["temperature"] = 0;
    json_sensors["humidity"] = 0;
    json_sensors["gas_resistance"] = 0;
    json_sensors["pressure"] = 0;
  }
  if (init_status.rtc_){
  json_sensors["year_"] = data_->rtc_->year_;
  json_sensors["month_"] = data_->rtc_->month_;
  json_sensors["day_"] = data_->rtc_->day_;
  json_sensors["hour_"] = data_->rtc_->hour_;
  json_sensors["minute_"] = data_->rtc_->minute_;
  json_sensors["second_"] = data_->rtc_->second_;
  } else{
    json_sensors["year_"] = 0;
    json_sensors["month_"] = 0;
    json_sensors["day_"] = 0;
    json_sensors["hour_"] = 0;
    json_sensors["minute_"] = 0;
    json_sensors["second_"] = 0;
  }
  if (init_status.mpu_){
    json_sensors["ax"] = data_->mpu_->aX;
    json_sensors["ay"] = data_->mpu_->aY;
    json_sensors["az"] = data_->mpu_->aZ;
    json_sensors["gx"] = data_->mpu_->gX;
    json_sensors["gy"] = data_->mpu_->gY;
    json_sensors["gz"] = data_->mpu_->gZ;
    json_sensors["mx"] = data_->mpu_->mX;
    json_sensors["my"] = data_->mpu_->mY;
    json_sensors["mz"] = data_->mpu_->mZ;
  } else{
    json_sensors["ax"] = 0;
    json_sensors["ay"] = 0;
    json_sensors["az"] = 0;
    json_sensors["gx"] = 0;
    json_sensors["gy"] = 0;
    json_sensors["gz"] = 0;
    json_sensors["mx"] = 0;
    json_sensors["my"] = 0;
    json_sensors["mz"] = 0;
    }
  serializeJson(json_sensors, json_string);
  //Serial.println(json_string);
  return &json_string;
}


void print_sensors_data(pointer_of_sensors * data_){
  if (init_status.bme_){
    print_data(data_->bme_); 
  } else{
   Serial.println("BME not found"); 
   }
  if (init_status.mpu_){
    print_data(data_->mpu_);
  } else{
   Serial.println("MPU not found"); 
  }
  if (init_status.ads_){
    print_data(data_->ads_);
  }
  else{
    Serial.println("ADS not found"); 
  }
  if (init_status.rtc_){
    print_data(data_->rtc_);
  } else{
    Serial.println("RTC not found"); 
  }
  get_all_sensor_data(data_);
}
