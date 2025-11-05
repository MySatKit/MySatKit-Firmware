//for communication with Nano (ATmega328p) microcontroller - MySat auxiliary microcontroller
//and for controlling the STAR LED - MySat debug indicator

//Nano (ATmega328p) is used for the operation of subsystems - control of the MySat solar panels movement and other tasks
#pragma once
#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>

#define SIGNALLED_BRIGHTNESS 20
#define STARLED_BRIGHTNESS 65
#define STARLED_PWM_CHANNEL 0

#define STARLED_BRIGHTNESS 65
#define STARLED_PWM_CHANNEL 0
#define EEPROM_ADDR_STATE_MOTOR 0

bool stateMotor;
unsigned long lastMotorChange = 0;

const int LED = 14;  //MySat STAR LED
const int SIGNAL_LED = 2;  //MySat SIGNAL LED
const int NUM_LEDS = 1;

Adafruit_NeoPixel signalStrip(NUM_LEDS, SIGNAL_LED, NEO_GRB + NEO_KHZ800);

enum LedMode { LED_OFF,
               LED_SOLID,
               LED_BLINK };

struct SignalLedState {
  uint8_t r, g, b;
  LedMode mode;
  uint8_t brightness;
  uint16_t interval;
  unsigned long lastUpdate;
  bool state;
};

SignalLedState signalLed;

void initSignalLed() {   //initializes the SIGNAL LED; used in setup()
  signalStrip.begin();
  signalStrip.setBrightness(SIGNALLED_BRIGHTNESS);
  signalStrip.show();
  signalLed = { 0, 0, 0, LED_OFF, SIGNALLED_BRIGHTNESS, 500, millis(), false };
}

void setSignalLed(uint8_t r, uint8_t g, uint8_t b, LedMode mode,        //configures the SIGNAL LED settings
                  uint8_t brightness = SIGNALLED_BRIGHTNESS, uint16_t interval = 500) {
  signalLed.r = r;
  signalLed.g = g;
  signalLed.b = b;
  signalLed.mode = mode;
  signalLed.brightness = brightness;
  signalLed.interval = interval;
  signalLed.lastUpdate = millis();
  signalLed.state = true;  //used within LED_BLINK mode to create the blinking effect
  signalStrip.setBrightness(brightness);
}

void updateSignalLed() {      //tracks which LED mode should be used; called in the loop() function
  unsigned long now = millis();
  switch (signalLed.mode) {
    case LED_OFF:
      signalStrip.clear();
      break;
    case LED_SOLID:
      signalStrip.setPixelColor(0, signalStrip.Color(signalLed.r, signalLed.g, signalLed.b));
      break;
    case LED_BLINK:
      if (now - signalLed.lastUpdate > signalLed.interval) {
        signalLed.state = !signalLed.state;
        signalLed.lastUpdate = now;
      }
      signalStrip.setPixelColor(0, signalLed.state ? signalStrip.Color(signalLed.r, signalLed.g, signalLed.b) : 0);
      break;
  }
  signalStrip.show();
}

void evaluateSystemState() {     //monitors the system state and triggers appropriate LED indication; called in loop()
  if (WiFi.status() != WL_CONNECTED) {
    setSignalLed(255, 255, 0, LED_BLINK, SIGNALLED_BRIGHTNESS, 200);
    return;
  }

  setSignalLed(0, 0, 255, LED_SOLID);
}

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
  } else {
    Wire.beginTransmission(8);
    Wire.write(byte(1));
    Wire.endTransmission();
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