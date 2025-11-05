//for BME680 - MySat environment (pressure, temperature etc.) sensor

//used for external observations and control of the internal state of the satellite
#pragma once

#include "bsec.h"
#include <Wire.h>
#include <Preferences.h> 
#define BME680_I2C_ADDR 0x77 

Bsec iaqSensor;

struct bme_struct{
    float temperature;
    float humidity;
    float gas_resistance; 
    float pressure;
    float iaq;      
    int iaq_accuracy;
    bool data_available = false; // for warming up sensor in the beginning and not to send just 0
    bool first_save_done = false; // for first save data if sensor do not work for 1 hour(save data by timer in loop())
} bme_data;

Preferences preferences;

void loadState(Bsec &iaqSensor) {
  preferences.begin("bsec_state", false);
  if (preferences.isKey("bsec_state")) {
    uint8_t state[BSEC_MAX_STATE_BLOB_SIZE];
    size_t n = preferences.getBytes("bsec_state", state, BSEC_MAX_STATE_BLOB_SIZE);
    
    iaqSensor.setState(state); 
    
    Serial.println("BSEC State loaded from NVS.");
  } else {
    Serial.println("BSEC State not found, starting fresh calibration.");
  }
  preferences.end();
}

void saveState(Bsec &iaqSensor) {
  preferences.begin("bsec_state", false);
  uint8_t state[BSEC_MAX_STATE_BLOB_SIZE];
  uint8_t state_len = BSEC_MAX_STATE_BLOB_SIZE; 
  iaqSensor.getState(state); 
  preferences.putBytes("bsec_state", state, state_len);
  preferences.end();
  Serial.println("BSEC state saved to NVS.");
}

bool initBME(){
  iaqSensor.begin(BME680_I2C_ADDR, Wire);
  
  if (iaqSensor.bsecStatus != BSEC_OK) {
    return false;
  }

  bsec_virtual_sensor_t sensorList[] = {
      BSEC_OUTPUT_IAQ, 
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE, 
      BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,   
      BSEC_OUTPUT_RAW_PRESSURE,
      BSEC_OUTPUT_RAW_GAS
  };
  
  iaqSensor.updateSubscription(sensorList, 5, BSEC_SAMPLE_RATE_CONT);
  loadState(iaqSensor);
  return true;
}

bme_struct * get_bme_data(){
    
    if (iaqSensor.run()) { 
        bme_data.iaq = iaqSensor.iaq;
        bme_data.iaq_accuracy = iaqSensor.iaqAccuracy;
        bme_data.temperature = iaqSensor.temperature;
        bme_data.humidity = iaqSensor.humidity;
        bme_data.pressure = iaqSensor.pressure / 100.0; 
        bme_data.gas_resistance = iaqSensor.gasResistance / 1000.0; 

        bme_data.data_available = true;

        if(!bme_data.first_save_done){
          saveState(iaqSensor);
          bme_data.first_save_done = true;
        }
    }
    return &bme_data;
}
