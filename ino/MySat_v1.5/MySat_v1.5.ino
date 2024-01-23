#include <Wire.h>
#include "server.h"


const int def_SDA(15);
const int def_SCL(13);

struct sensors_structure{
  float ph1;
  float ph2;
  float ph3;
  float ph4;
  float temperature;
  float humidity;
  float gas_resistance;
  float pressure;
  int year_;
  int month_;
  int day_;
  int hour_;
  int minute_;
  int second_; 
} sensors_data;

void setup() {
  Serial.begin(115200);
  Wire.begin(def_SDA, def_SCL);
  initSensors();
  initServer();
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
}
void loop() {
  
  Serial.println("Start loop");
  
  print_sensors_data(get_sensors_data());
  server.handleClient();
  Serial.println(WiFi.localIP());
   
  delay(1000);
}
