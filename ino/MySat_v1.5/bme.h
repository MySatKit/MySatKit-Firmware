#include "Adafruit_BME680.h"

Adafruit_BME680 bme;  //BME create

struct bme_struct{
   float temperature;
   float humidity;
   float gas_resistance;
   float pressure;
} bme_data;

bool initBME(){
  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    return false;
  }
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150);  
  return true;
}

bme_struct * get_bme_data(){
  bme_data.temperature = bme.readTemperature();
  bme_data.pressure = bme.readPressure() / 100;
  bme_data.humidity = bme.readHumidity();
  bme_data.gas_resistance = bme.readGas() / 1000.0;
  return &bme_data;
}

void print_data(bme_struct * bme680){
    Serial.print("Temperature = ");
    Serial.print(bme680->temperature);
    Serial.println(F(" *C"));
    
    Serial.print("Pressure = ");
    Serial.print(bme680->pressure);
    Serial.println(" hPa");

    Serial.print("Humidity = ");
    Serial.print(bme680->humidity);
    Serial.println(" %");

    Serial.print("Gas = ");
    Serial.print(bme680->gas_resistance);
    Serial.println(" KOhms");
}
