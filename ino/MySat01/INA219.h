#pragma once

#include "Adafruit_INA219.h" //https://github.com/Makuna/Rtc

static Adafruit_INA219 ina219;

void initINA219()
{
  Serial.print("Check INA219...");
  bool status = ina219.begin(&getI2C());  
}

Adafruit_INA219& getINA219()
{
    return ina219;
}
