#include <Wire.h>
#include <WiFi.h>



#define I2C_SDA 2
#define I2C_SCL 4
TwoWire I2Cnew = TwoWire(0);


const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
// WiFi network info.
const char *ssid =  "TP-LINK_87CC";     // Enter your WiFi Name
const char *pass =  "Kelep1632"; // Enter your WiFi Password
WiFiServer server(80);
void setup(){
 Serial.begin(115200);
  Serial.println("\nI2C Scanner");

   I2Cnew.begin(I2C_SDA, I2C_SCL, 400000);
}





 
void loop() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    I2Cnew.beginTransmission(address);
    error = I2Cnew.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  delay(5000);          
}
