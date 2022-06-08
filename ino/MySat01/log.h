#pragma once

#include "Adafruit_INA219.h" //https://github.com/Makuna/Rtc
#include <MPU9250_asukiaaa.h> // http://www.esp32learning.com/code/esp32-and-mpu-9250-sensor-example.php
#include <Adafruit_BME280.h>
#include <Adafruit_ADS1X15.h>
#include <RtcDS3231.h>
#include "WiFi.h"

#define SEALEVELPRESSURE_HPA (1013.25)

namespace Log
{
    void printValuesINA219(Adafruit_INA219 &ina219)
    {
        float shuntvoltage = ina219.getShuntVoltage_mV();
        float busvoltage = ina219.getBusVoltage_V();
        float current = ina219.getCurrent_mA();
        float power = ina219.getPower_mW();

        Serial.print("BV"); Serial.print("\t"); // Bus Voltage
        Serial.print("SV"); Serial.print("\t"); // Shunt Voltage
        Serial.print("LV"); Serial.print("\t"); // Load Voltage
        Serial.print("C"); Serial.print("\t");  // Current
        Serial.println("P");  // Power

        Serial.print(busvoltage); Serial.print("\t"); 
        Serial.print(shuntvoltage); Serial.print("\t");
        Serial.print(current); Serial.print("\t");
        Serial.println(power);
    }

    void printValuesMPU9250(MPU9250_asukiaaa &MPU9250) 
    {
        uint8_t sensorId = 0x69;
        if (MPU9250.readId(&sensorId) == 0) 
            Serial.println("sensorId: " + String(sensorId));
        else 
            Serial.println("Cannot read sensorId");
        
        if (MPU9250.accelUpdate() == 0) 
        {
            float aX = MPU9250.accelX();
            float aY = MPU9250.accelY();
            float aZ = MPU9250.accelZ();
            float aSqrt = MPU9250.accelSqrt();
            Serial.println("accelX: " + String(aX));
            Serial.println("accelY: " + String(aY));
            Serial.println("accelZ: " + String(aZ));
            Serial.println("accelSqrt: " + String(aSqrt));
        } 
        else
            Serial.println("Cannod read accel values");

        if (MPU9250.gyroUpdate() == 0) 
        {
            float gX = MPU9250.gyroX();
            float gY = MPU9250.gyroY();
            float gZ = MPU9250.gyroZ();
            Serial.println("gyroX: " + String(gX));
            Serial.println("gyroY: " + String(gY));
            Serial.println("gyroZ: " + String(gZ));
        } 
        else 
            Serial.println("Cannot read gyro values");

        if (MPU9250.magUpdate() == 0) 
        {
            float mX = MPU9250.magX();
            float mY = MPU9250.magY();
            float mZ = MPU9250.magZ();
            float mDirection = MPU9250.magHorizDirection();
            Serial.println("magX: " + String(mX));
            Serial.println("maxY: " + String(mY));
            Serial.println("magZ: " + String(mZ));
            Serial.println("horizontal direction: " + String(mDirection));
        } 
        else 
            Serial.println("Cannot read mag values");
    }

    void printValuesBME(Adafruit_BME280 &bme) 
    {
        Serial.print("Temperature = ");
        Serial.print(bme.readTemperature());
        Serial.println(" *C");

        Serial.print("Pressure = ");

        Serial.print(bme.readPressure() / 100.0F);
        Serial.println(" hPa");

        Serial.print("Approx. Altitude = ");
        Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
        Serial.println(" m");

        Serial.print("Humidity = ");
        Serial.print(bme.readHumidity());
        Serial.println(" %");

        Serial.println();
    }

    void printValuesADS(Adafruit_ADS1015 &ads) 
    {
        Serial.print("ph1 = ");
        Serial.println(ads.readADC_SingleEnded(0));

        Serial.print("ph2 = ");
        Serial.println(ads.readADC_SingleEnded(1));
        
        Serial.print("ph3 = ");
        Serial.println(ads.readADC_SingleEnded(2));
        
        Serial.print("ph4 = ");
        Serial.println(ads.readADC_SingleEnded(3));
        Serial.println();
        
    }

    void printDateTimeS(const RtcDateTime& dt)
    {
        char datestring[20];

        snprintf_P(datestring, 
                20,
                PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
                dt.Month(),
                dt.Day(),
                dt.Year(),
                dt.Hour(),
                dt.Minute(),
                dt.Second() );
        String sTime = String(datestring);
        Serial.print(sTime);
    }

    void checkStatus(bool status)
    {
        if(status)
            Serial.println(" Done");
        else
            Serial.println(" Fail");
    }
 
    void printWiFiInfo()
    {
        // Print ESP32 Local IP Address
        Serial.println();    
        Serial.print("RSSI:");
        Serial.println(WiFi.RSSI());  
        Serial.print("IP Address: http://");
        Serial.println(WiFi.localIP());
        Serial.println("End Init");
        Serial.println("");
    }
}
