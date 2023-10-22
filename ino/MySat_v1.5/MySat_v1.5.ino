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
/*
const size_t bufferSize = JSON_OBJECT_SIZE(14);

DynamicJsonDocument json_sensors(bufferSize);
String json_string;

*/

void setup() {
  Serial.begin(115200);
  Wire.begin(def_SDA, def_SCL);
  initSensors();
  initServer();
  //init_camera();
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
}
void loop() {
  
  Serial.println("Start loop");
  
  //NO String * json_ = get_all_sensor_data(bme_temp, mpu_temp, ads_temp, rtc_temp);
  
  print_sensors_data(get_sensors_data());
  
  
  server.handleClient();
  
  Serial.println(WiFi.localIP());
   
  delay(1000);
}
