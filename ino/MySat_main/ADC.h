//for ADS1015 - analog-to-digital convertor (ADC) on MySat

//used to process values ​​from photosensors in the solar navigation system
#pragma once

#include <Adafruit_ADS1X15.h>

Adafruit_ADS1015 ads;
int setup_;

struct ads_struct{
   float ph1;
   float ph2;
   float ph3;
   float ph4;
} ads_data;

bool initADS(){
  int setup_ = ads.begin(0x48, &Wire);
  //Serial.print("ADS1015 module (analog-to-digatal convertor) status: ");
  if(!setup_){
    return false;
  } else{
    return true;
  }
}

ads_struct * get_ads_data(){
  ads_data.ph1 = ads.readADC_SingleEnded(0); 
  ads_data.ph2 = ads.readADC_SingleEnded(1); 
  ads_data.ph3 = ads.readADC_SingleEnded(2); 
  ads_data.ph4 = ads.readADC_SingleEnded(3);
  return &ads_data;
}