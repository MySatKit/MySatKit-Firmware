//for communication with Arduino Nano  - MySat auxiliary microcontroller
//and for controlling the STAR LED - MySat debug indicator

//Arduino Nano is used for the operation of subsystems - control of the MySat solar panels movement and other tasks


const int LED = 14;  //MySat STAR LED

void control_light(int open) {
 if(open % 2 == 1){
  digitalWrite(LED, HIGH);
  }
 else{
  digitalWrite(LED, LOW);
  }
 }



void control_motor(int open_motor) {
  if (open_motor % 2 == 1){
    Wire.beginTransmission(8);
    Wire.write(byte(0));
    Wire.endTransmission();
    Serial.println("Open motor");
  }
 else {
    Wire.beginTransmission(8);
    Wire.write(byte(1));
    Wire.endTransmission();
    Serial.println("Close motor");
  }
 }