//for INA3221 - triple-channel current and voltage sensor 

//used to control battery charge and solar panels current

#include <Beastdevices_INA3221.h>

Beastdevices_INA3221 ina(INA3221_ADDR40_GND);

struct ina_struct {
  float batteryVoltage;
  float batteryCurrent;
  float SolarPanelVoltage;
  //float leftSolarPanelRelCurrent;
  //float rightSolarPanelRelCurrent;
  float leftSolarPanelCurrent;
  float rightSolarPanelCurrent;
} ina_data;

bool initINA() {
  ina.begin();
  return true;
}

//
// CH2 = LEFT
// CH3 = RIGHT

ina_struct* get_ina_data() {
  ina_data.batteryVoltage = ina.getVoltage(INA3221_CH1);
  ina_data.batteryCurrent = ina.getCurrent(INA3221_CH1);

  ina_data.SolarPanelVoltage  = (ina.getVoltage(INA3221_CH2) + ina.getVoltage(INA3221_CH3)) / 2;

  ina_data.leftSolarPanelCurrent = ina.getCurrent(INA3221_CH2);
  ina_data.rightSolarPanelCurrent = ina.getCurrent(INA3221_CH3);

  /*if (ina.getCurrent(INA3221_CH2) +  ina.getCurrent(INA3221_CH3) > 0.01) {
    ina_data.leftSolarPanelRelCurrent = ina.getCurrent(INA3221_CH2) / (ina.getCurrent(INA3221_CH2) + ina.getCurrent(INA3221_CH3)) * 100;
    ina_data.rightSolarPanelRelCurrent = ina.getCurrent(INA3221_CH3) / (ina.getCurrent(INA3221_CH2) + ina.getCurrent(INA3221_CH3)) * 100;
  }
  else {
    ina_data.leftSolarPanelRelCurrent = 0;
    ina_data.rightSolarPanelRelCurrent = 0;
  }*/
  return &ina_data;
}

void print_data(ina_struct* data_) {

  float IL = data_->leftSolarPanelCurrent;
  float IR = data_->rightSolarPanelCurrent;
  float relIL, relIR;

  Serial.println("===POWER SYSTEM:==================");
  Serial.print("  Battery:      U = "); 
  Serial.print(data_->batteryVoltage, 2);   
  Serial.print("  V,  I =  "); 
  Serial.print(data_->batteryCurrent, 2); 
  Serial.println(" mA");

  //if (data_->SolarPanelVoltage > 0.03 ) {
    Serial.print("  Solar Panels: U = "); 
    Serial.print(data_->SolarPanelVoltage, 2); Serial.print(" V,");

    Serial.println(  RAW DATA:);
    Serial.print("IL = "); Serial.print(IL); Serial.print("   IR = "); Serial.println(IR);

    if (IL < 0) IL = 0;
    if (IR < 0) IR = 0;
    
    if (IL + IR > 0.01) {
      relIL = IL / (IL+IR);
      relIR = IR / (IL+IR);
    }
    else {
      relIL = 0;
      relIL = 0;
    }

    Serial.println(  PROCESSED:);

    Serial.print("  I(L / R) = "); 
    Serial.print(IL); Serial.print("% / "); Serial.print(IR); Serial.println("%");

    //Serial.print(data_->leftSolarPanelRelCurrent, 0); Serial.print("% / "); 
    //Serial.print(data_->rightSolarPanelRelCurrent, 0); Serial.println("%");

    Serial.println();

    Serial.println(  REAL DATA:);


  //}
  //else {
  //  Serial.println("▲ No Solar panels conncted"); 
  //}
  

  //DEBUG:
  Serial.print("I L = "); Serial.print(ina.getCurrent(INA3221_CH2)); Serial.print("  I R = "); Serial.println(ina.getCurrent(INA3221_CH3));
}
