//for INA3221 - voltage and current sensor 

//used to control battery charge and solar panels voltage

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

void print_data(ina_struct* data_) {

  float IL = data_->leftSolarPanelCurrent;
  float IR = data_->rightSolarPanelCurrent;
  if (IL < 0) IL = 0;
  if (IR < 0) IR = 0;

  Serial.println("===POWER SYSTEM:==================");
  Serial.print("  Battery:      U = "); 
  Serial.print(data_->batteryVoltage, 2);   
  Serial.print("  V,  I =  "); 
  Serial.print(data_->batteryCurrent, 2); 
  Serial.println(" mA");

  Serial.print("  Solar Panels: U = "); 
  Serial.print(data_->SolarPanelVoltage, 2); Serial.print("  V,");

  if (IL + IR > 5) {
    Serial.print("  I_L = "); Serial.print(IL, 2); Serial.print(" mA,  I_R = "); Serial.print(IR, 2); Serial.println(" mA");
  }
  else {
    Serial.println("  I =  low ");
  }
  
  //ina.reset();
}
