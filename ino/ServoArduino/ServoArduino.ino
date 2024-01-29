#include <Wire.h>
#include <Servo.h> 
Servo servo;
int angle = 180;

void setup() {
 pinMode(LED_BUILTIN, OUTPUT);
 Wire.begin(8);               
 Wire.onReceive(receiveEvent); 
 Wire.onRequest(requestEvent); 
 Serial.begin(115200);
  servo.attach(9);
  servo.write(angle);
}
byte arr[] = {110, 0}; 

void loop() {
  if(arr[0] == 0){
  for(int angle = 0; angle < 180; angle++)  
  {                                  
    servo.write(angle);             
    delay(10);
    Serial.println("RUN->");                   
  }
  arr[0] = 2;
  }
  if(arr[0] == 1){
  for(int angle = 180; angle > 0; angle--)  
  {                                  
    servo.write(angle);             
    delay(10);
    Serial.println("<-RUN");                   
  }
  arr[0] = 2;
  }
 delay(1000);
 Serial.println("END");

  
}

void receiveEvent(int howMany) {
  int counter = 0;
  while (0 < Wire.available()) {
    
    byte c = Wire.read();
    arr[0] = c;
    counter++;
  }
 Serial.println(arr[0]);
 Serial.println(arr[1]);

}
int x = 90; 
void requestEvent() {
 
 Wire.write(byte(x));
 x++;
}
