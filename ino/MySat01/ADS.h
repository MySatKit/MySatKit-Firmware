#pragma once
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include "I2C.h"

static Adafruit_ADS1015 ads;

void initADS()
{
  Serial.print("Check ADS...");
  bool status = ads.begin(0x48, &getI2C());  
  Log::checkStatus(status);
}

void getLightSensorReadings(float &ph1, float &ph2, float &ph3, float &ph4)
{
  ph1 = ads.readADC_SingleEnded(0); 
  ph2 = ads.readADC_SingleEnded(1); 
  ph3 = ads.readADC_SingleEnded(2); 
  ph4 = ads.readADC_SingleEnded(3);
}

Adafruit_ADS1015& getADS1015()
{
    return ads;
}
