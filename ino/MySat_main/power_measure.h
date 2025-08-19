//for INA3221 - triple-channel current and voltage sensor 

//used to control battery charge and solar panels current

#include <Beastdevices_INA3221.h>

Beastdevices_INA3221 ina(INA3221_ADDR40_GND);

struct ina_struct {
  float batteryVoltage;
  float batteryCurrent;
  float leftSolarPanelPower;
  float rightSolarPanelPower;
} ina_data;

bool initINA() {
  ina.begin();
  return true;
}

ina_struct* get_ina_data() {
  ina_data.batteryVoltage = ina.getVoltage(INA3221_CH1);
  ina_data.batteryCurrent = ina.getCurrent(INA3221_CH1);
  ina_data.leftSolarPanelPower  = ina.getVoltage(INA3221_CH2) * ina.getCurrent(INA3221_CH2);
  ina_data.rightSolarPanelPower = ina.getVoltage(INA3221_CH3) * ina.getCurrent(INA3221_CH3);
  return &ina_data;
}

void print_data(ina_struct* data_) {
  Serial.println("===POWER SYSTEM:==================");
  Serial.print("  Battery:       U = "); 
  Serial.print(data_->batteryVoltage, 2);   
  Serial.print("  V   I =  "); 
  Serial.print(data_->batteryCurrent, 2); 
  Serial.println(" mA");

  Serial.print("  Solar Panel L: P = "); Serial.print(data_->leftSolarPanelPower, 2); Serial.println(" mW");
  Serial.print("  Solar Panel R: P = "); Serial.print(data_->rightSolarPanelPower, 2); Serial.println(" mW");
}
