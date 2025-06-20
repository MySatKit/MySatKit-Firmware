//for INA - triple-channel current and voltage sensore 

// used to monitor battery voltage/current and calculate solar panel power output

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
  Serial.println("═══POWER SYSTEM══════════════════");
  Serial.print("Battery:\n  Voltage = ");
  Serial.print(data_->batteryVoltage, 2);  // 
  Serial.print(" V\n  Current = ");
  Serial.print(data_->batteryCurrent, 2); 
  Serial.println(" mA");

  Serial.print("\nSolar Panel Left:\n  Power = ");
  Serial.print(data_->leftSolarPanelPower, 2);
  Serial.println(" mW");

  Serial.print("Solar Panel Right:\n  Power = ");
  Serial.print(data_->rightSolarPanelPower, 2);
  Serial.println(" mW");

  Serial.println("════════════════════════════════");
}
