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

void initSensors(){
  initBME();
  initMPU();
  initADS();
  initRTC();
  init_camera();
}

pointer_of_sensors * get_sensors_data(){
  pointers.bme_ = get_bme_data();
  pointers.mpu_ = get_mpu_data();
  pointers.ads_ = get_ads_data();
  pointers.rtc_ = get_rtc();
  
  //camera_fb_t * fb = get_photo();
  //Serial.print("Camera len: ");
  //Serial.println(fb->len);
  return &pointers;
}

String * get_all_sensor_data(pointer_of_sensors * data_){
  json_string = "";
  json_sensors["ph1"] = data_->ads_->ph1;
  json_sensors["ph2"] = data_->ads_->ph2;
  json_sensors["ph3"] = data_->ads_->ph3;
  json_sensors["ph4"] = data_->ads_->ph4;
  Serial.print("TEMPP  ");
  Serial.println(data_->bme_->temperature);
  json_sensors["temperature"] = data_->bme_->temperature;
  json_sensors["humidity"] = data_->bme_->humidity;
  json_sensors["gas_resistance"] = data_->bme_->gas_resistance;
  json_sensors["pressure"] = data_->bme_->pressure;
  json_sensors["year_"] = data_->rtc_->year_;
  json_sensors["month_"] = data_->rtc_->month_;
  json_sensors["day_"] = data_->rtc_->day_;
  json_sensors["hour_"] = data_->rtc_->hour_;
  json_sensors["minute_"] = data_->rtc_->minute_;
  json_sensors["second_"] = data_->rtc_->second_;
  
  json_sensors["ax"] = data_->mpu_->aX;
  json_sensors["ay"] = data_->mpu_->aY;
  json_sensors["az"] = data_->mpu_->aZ;
  json_sensors["gx"] = data_->mpu_->gX;
  json_sensors["gy"] = data_->mpu_->gY;
  json_sensors["gz"] = data_->mpu_->gZ;
  json_sensors["mx"] = data_->mpu_->mX;
  json_sensors["my"] = data_->mpu_->mY;
  json_sensors["mz"] = data_->mpu_->mZ;
  
  serializeJson(json_sensors, json_string);
  //Serial.println(json_string);
  return &json_string;
}


void print_sensors_data(pointer_of_sensors * data_){;
  print_data(data_->bme_);
  print_data(data_->mpu_);
  print_data(data_->ads_);
  print_data(data_->rtc_);
  get_all_sensor_data(data_);
}
