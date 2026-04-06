/*
 * MYSAT FIRMWARE 
 * for Nano board (ATmega328p)			! select "Arduino Nano" in Arduino IDE Boards Manager
 *
 *
 * Controls the satellite subsystems in accordance with commands from the ESP32-CAM (main satellite OBC):
 *  - servomotor (MySat solar panels deployment);
 *  - HC-12 module (MySat radio transceiver);
 *
 * version: v.1.2.0
 * date: 23.09.2025
 *
 *
 */

#define VERSION "v.1.2.0"

#include <Wire.h>
#include <Servo.h> 

enum Commands_list {
    MOTOR_OPEN,
    MOTOR_CLOSE,
    RF_TURN,
    RF_SET
};
Commands_list command;
bool command_flag = 0;

//  Servo motor angles:
const uint8_t CLOSED_ANGLE = 170; //[degrees]
const uint8_t OPENED_ANGLE = 10;
//  Delays for timers:
const int POWER_SUPPLY_DELAY = 2200; //[ms]
const int STEP_DELAY = 10;

//Servo settings:
const uint8_t MOTOR_PIN = 9;  //using D9 pin of Nano board to control the servomotor
Servo MySat_servo; //creating servo object
bool direction;
uint8_t angle = CLOSED_ANGLE;
enum Motor_statuses { //all possible servo motor statuses
    NEW_COMMAND,
    TURNING,
    DONE,
    OFF
};
Motor_statuses motor_status = OFF;

//RF transceiver settings:
const uint8_t RF_ON_PIN = 5;  //using D5 pin of Nano board to control the HC-12 power supply
const uint8_t RF_SET_PIN = 4; //using D5 pin of Nano board to control the HC-12 set pin


void ESP32_I2C_handler(); //called automatically when receiving a signal via the I2С from ESP32
void command_handler(); //processes commands received in ESP32_I2C_handler()
void blink_LED(); //blinks the built-in LED

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RF_ON_PIN, OUTPUT);
  digitalWrite(RF_ON_PIN, LOW); //allows HC-12 to always work by default (necessary for boards v.1.5.5)
  //  I2C settings:
  Wire.begin(0x08); 
  Wire.onReceive(ESP32_I2C_handler); 
  //  Serial port:
  Serial.begin(115200);
  Serial.print("MYSAT Firmware "); Serial.println(VERSION);
  Serial.println("Awaiting commands from ESP32...");
}

void loop() {
    command_handler();
    motor();
    blink_LED();
}




void ESP32_I2C_handler() {
  while (0 < Wire.available()) {
    command = Wire.read();
  }
  command_flag = true;
}

void command_handler() {
  if (command_flag == true) {
    command_flag = false;
    switch(command) {
      case MOTOR_CLOSE:
      case MOTOR_OPEN:
        if (motor_status == OFF) {
          motor_status = NEW_COMMAND;
          direction = command; //save the command for turning operation
        }
        break;
      case RF_TURN:
      case RF_SET:
        break;
    }
  }
}

void motor() {
  static unsigned long power_supply_timer;
  static unsigned long step_timer;
  switch (motor_status) {

    case NEW_COMMAND: //turn on the power to the motor
      Serial.print("Motor: get new command -> "); Serial.println(command);
      motor_status = TURNING;
      MySat_servo.attach(MOTOR_PIN);
      power_supply_timer = millis();
      step_timer = millis();
      Serial.println("  Start turning...");
      break;

    case TURNING:
      if ((millis() - power_supply_timer) < POWER_SUPPLY_DELAY) { //power is supplied for only 2.2 seconds to avoid motor overheating due to rattling
        if ((millis() - step_timer) > STEP_DELAY) { //add a delay after each step for smoother turning

          if ((direction == MOTOR_CLOSE && angle < CLOSED_ANGLE) ||
              (direction == MOTOR_OPEN  && angle > OPENED_ANGLE)) {
  
                angle += (direction == MOTOR_CLOSE) ? 1 : -1;
                MySat_servo.write(angle);
                step_timer = millis();

          } else { //angle reached?
              motor_status = DONE;
              Serial.println((direction == MOTOR_CLOSE) ? "  Closing done." : "  Opening done.");
          }
        }
      }
      else { //out of POWER_SUPPLY_DELAY?
        motor_status = DONE;
        Serial.println("  Detaching by timer!");
      }
      break;

    case DONE: //turn off the power to the motor
      MySat_servo.detach();
      Serial.println("  Servo off.");
      motor_status = OFF;
      break;

    case OFF: //standby mode
      break;
  }
}

void blink_LED() { //blinking the built-in LED according to the pattern: -----*--*-----
  static const uint16_t pattern[] = {400, 200, 100, 3000}; // [ms]
  static uint8_t step = 0;
  static unsigned long timer = 0;
  if (millis() - timer > pattern[step]) {
    digitalWrite(LED_BUILTIN, step % 2); // 0 - LOW, 1 - HIGH
    timer = millis();
    step = (step + 1) % 4;
  }
}
