#pragma once
#include <MPU9250_asukiaaa.h> // http://www.esp32learning.com/code/esp32-and-mpu-9250-sensor-example.php
#include "I2C.h"

static MPU9250_asukiaaa MPU9250(0x69);

void initMPU9250()
{
  Serial.print("Check MPU...");
  MPU9250.setWire(&getI2C());
  MPU9250.beginAccel();
  MPU9250.beginGyro();
  MPU9250.beginMag();
  Serial.println(" TBD.");
} 

MPU9250_asukiaaa& getMPU9250()
{
    return MPU9250;
}
