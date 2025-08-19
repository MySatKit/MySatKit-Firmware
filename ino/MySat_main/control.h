//for communication with Nano (ATmega328p) microcontroller - MySat auxiliary microcontroller
//and for controlling the STAR LED - MySat debug indicator

//Nano (ATmega328p) is used for the operation of subsystems - control of the MySat solar panels movement and other tasks
#include <EEPROM.h>

#define STARLED_BRIGHTNESS 65
#define STARLED_PWM_CHANNEL 0
#define EEPROM_ADDR_STATE_MOTOR 0

bool stateMotor;
unsigned long lastMotorChange = 0;

const int LED = 14;  //MySat STAR LED

void control_light(bool state_light) {  //turns the STAR LED on or off based on the input parameter (true/false)
  if (state_light) {
    ledcWrite(STARLED_PWM_CHANNEL, STARLED_BRIGHTNESS);
  } else {
    ledcWrite(STARLED_PWM_CHANNEL, 0);
  }
}

void initStarLed() {  //initializes the STAR LED; used in setup()
  ledcSetup(STARLED_PWM_CHANNEL, 5000, 8);
  ledcAttachPin(LED, STARLED_PWM_CHANNEL);
  control_light(false);
}

void control_motor(bool state_motor) {  //deploys or retracts the solar panels depending on the input parameter (true/false)
  if (state_motor) {
    Wire.beginTransmission(8);
    Wire.write(byte(0));
    Wire.endTransmission();
    Serial.println("Close motor");
  } else {
    Wire.beginTransmission(8);
    Wire.write(byte(1));
    Wire.endTransmission();
    Serial.println("Open motor");
  }
}

void loadStateMotor() {
  EEPROM.begin(4);  
  byte val = EEPROM.read(EEPROM_ADDR_STATE_MOTOR);
  stateMotor = (val == 1);
}

void saveStateMotor() {
  EEPROM.write(EEPROM_ADDR_STATE_MOTOR, stateMotor ? 1 : 0);
  EEPROM.commit(); 
}

bool setStateMotor(bool newState) {
  unsigned long now = millis();
  if (now - lastMotorChange < 2200) return false;  
  if (stateMotor != newState) {
    stateMotor = newState;
    saveStateMotor();
    control_motor(stateMotor);
    lastMotorChange = now;
    return true;
  }
  return false;
}