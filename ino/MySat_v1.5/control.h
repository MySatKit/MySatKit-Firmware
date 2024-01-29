 const int LED = 1000;
 
 void control_light(int open){
 if(open % 2 == 1){
  digitalWrite(LED, HIGH);
  }
 else{
  digitalWrite(LED, LOW);
  }
 }

  void control_motor(int open_motor){
 if(open_motor % 2 == 1){
  Wire.beginTransmission(8);
  Wire.write(byte(0));
  Wire.endTransmission();
  Serial.println("Open motor");
  }
 else{
  Wire.beginTransmission(8);
  Wire.write(byte(1));
  Wire.endTransmission();
  Serial.println("Close motor");
  }
 }
