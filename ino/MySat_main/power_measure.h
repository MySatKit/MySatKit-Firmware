//for INA3221 - voltage and current sensor 

//used to control battery charge and solar panels voltage
#pragma once

#include <Beastdevices_INA3221.h>

Beastdevices_INA3221 ina(INA3221_ADDR40_GND);

struct ina_struct {
  float batteryVoltage;
  float batteryCurrent;
  float SolarPanelVoltage;
  float leftSolarPanelCurrent;
  float rightSolarPanelCurrent;
} ina_data;

bool initINA() {
  ina.begin();
  ina.setShuntRes(100, 100, 100);
  return true;
}

//
// CH2 = LEFT
// CH3 = RIGHT
//

ina_struct* get_ina_data() {
  ina_data.batteryVoltage = ina.getVoltage(INA3221_CH1);
  ina_data.batteryCurrent = ina.getCurrent(INA3221_CH1) * 1000;

  ina_data.SolarPanelVoltage  = (ina.getVoltage(INA3221_CH2) + ina.getVoltage(INA3221_CH3)) / 2;

  ina_data.leftSolarPanelCurrent = ina.getCurrent(INA3221_CH2) * 1000;
  ina_data.rightSolarPanelCurrent = ina.getCurrent(INA3221_CH3) * 1000;
  return &ina_data;
}
