#pragma once

 void printValuesINA219()
    {
        shuntvoltage = ina219.getShuntVoltage_mV();
        busvoltage = ina219.getBusVoltage_V();
        current = ina219.getCurrent_mA();
        power = ina219.getPower_mW();
        loadvoltage = busvoltage + (shuntvoltage / 1000);

        Serial.print("BV"); Serial.print("\t"); // Bus Voltage
        Serial.print("SV"); Serial.print("\t"); // Shunt Voltage
        Serial.print("LV"); Serial.print("\t"); // Load Voltage
        Serial.print("C"); Serial.print("\t");  // Current
        Serial.println("P");  // Power

        Serial.print(busvoltage); Serial.print("\t"); 
        Serial.print(shuntvoltage); Serial.print("\t");
        Serial.print(loadvoltage); Serial.print("\t");
        Serial.print(current); Serial.print("\t");
        Serial.println(power);
    }

namespace Log
{
    void printMessageInitialization(bool status) 
    {
        if (status)
            Serial.println(" Done.");
        else
            Serial.println(" Fail");
    }
    
    void printValues9250() 
    {  
    uint8_t sensorId = 0x69;
    if (mySensor.readId(&sensorId) == 0)
        Serial.println("sensorId: " + String(sensorId));
    else 
        Serial.println("Cannot read sensorId");
 
    if (mySensor.accelUpdate() == 0) 
    {
        aX = mySensor.accelX();
        aY = mySensor.accelY();
        aZ = mySensor.accelZ();
        aSqrt = mySensor.accelSqrt();
        Serial.println("accelX: " + String(aX));
        Serial.println("accelY: " + String(aY));
        Serial.println("accelZ: " + String(aZ));
        Serial.println("accelSqrt: " + String(aSqrt));
    } 
    else 
        Serial.println("Cannod read accel values");
 
    if (mySensor.gyroUpdate() == 0) 
    {
        gX = mySensor.gyroX();
        gY = mySensor.gyroY();
        gZ = mySensor.gyroZ();
        Serial.println("gyroX: " + String(gX));
        Serial.println("gyroY: " + String(gY));
        Serial.println("gyroZ: " + String(gZ));
    }
    else 
        Serial.println("Cannot read gyro values");
 
    if (mySensor.magUpdate() == 0) 
    {
        mX = mySensor.magX();
        mY = mySensor.magY();
        mZ = mySensor.magZ();
        mDirection = mySensor.magHorizDirection();
        Serial.println("magX: " + String(mX));
        Serial.println("maxY: " + String(mY));
        Serial.println("magZ: " + String(mZ));
        Serial.println("horizontal direction: " + String(mDirection));
    } 
    else 
        Serial.println("Cannot read mag values");
    }

    void printValuesBME() 
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

    void printValuesPhotoRes()
    {
        Serial.println("Phores"); 
        Serial.print("Photo 1: ");
        Serial.println(ph1);

        Serial.print("Photo 2: ");
        Serial.println(ph2);

        Serial.print("Photo 3: ");
        Serial.println(ph3);

        Serial.print("Photo 4: ");
        Serial.println(ph4);
    }
}
