#pragma once

static Adafruit_BME280 bme;

void initBME()
{
  Serial.print("Check BME...");
  bool status = bme.begin(0x76, &getI2C());  
  Log::checkStatus(status);
}

void getSensorReadings(float &temperature, float &humidity, float &pressure)
{
  temperature = bme.readTemperature();
  // Convert temperature to Fahrenheit
  //temperature = 1.8 * bme.readTemperature() + 32;
  humidity = bme.readHumidity();
  pressure = bme.readPressure()/ 100.0F;
}

Adafruit_BME280& getBME280()
{
    return bme;
}
