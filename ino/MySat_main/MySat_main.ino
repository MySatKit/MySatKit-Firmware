//updated 06.10.2024

#include <Wire.h>
#include "server.h"
#include <SPIFFS.h>

String ssid = "";
String password = "";

bool loadWiFiConfig(String& ssid, String& password){  
  if(!SPIFFS.begin(true)){
    Serial.println("SPIFFS mount failed!");
    return false;
  }

  File file = SPIFFS.open("/config.txt", "r");
  if(!file){
    Serial.println("No config.txt found");
    return false;
  }

  ssid = file.readStringUntil('\n'); ssid.trim();
  password = file.readStringUntil('\n'); password.trim();
  file.close();
  return true;
}

void saveWiFiConfig(const String& ssid, const String& password){
  File file = SPIFFS.open("/config.txt", "w");
  if(!file){
    Serial.println("Can`t write config.txt");
    return;
  }
  file.println(ssid);
  file.println(password);
  file.close();
}

void promptUserForWiFi(String& ssid, String& password){
  Serial.println("Please enter WiFi ssid: ");
  while(ssid.length() == 0){
    if(Serial.available()){
      ssid = Serial.readStringUntil('\n'); ssid.trim();
    }
  }
  Serial.println("Please enter WiFi password: ");
  while(password.length() == 0){
    if(Serial.available()){
      password = Serial.readStringUntil('\n'); password.trim();
    }
  }
}

void tryConnectWiFi(){                                      //щоб не було дублювання коду, адже цю функцію я також використовую і в changeWiFiData()
  WiFi.begin(ssid.c_str(), password.c_str());

  int retry = 0;
  while(WiFi.status() != WL_CONNECTED && retry < 20){
    delay(500);
    Serial.print(".");
    retry++;
  }

  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Failed to connect! Please enter a new data:");
    ssid = "";
    password = "";
    promptUserForWiFi(ssid, password);
    saveWiFiConfig(ssid, password);

    WiFi.begin(ssid.c_str(), password.c_str());
    retry = 0;
    while(WiFi.status() != WL_CONNECTED && retry < 20){
      delay(500);
      Serial.print(".");
      retry++;
    }

    if(WiFi.status() == WL_CONNECTED){
      Serial.println("WiFi connected successfully!");
    }else{
      Serial.println("Please check and change your WiFi data!!!");
    }
  }else{
    Serial.println("Connected successfully!");
  }
}

void connectToWiFi(){
  if(!loadWiFiConfig(ssid, password)){
    Serial.println("No WiFi config. Enter data for WiFi");
    promptUserForWiFi(ssid, password);
    saveWiFiConfig(ssid, password);
  }

  Serial.print("Connecting to WiFi: ");
  Serial.print(ssid + " ...");

  tryConnectWiFi();
}

void useCommandForChanging() {
  static String inputBuffer = "";

  while (Serial.available() > 0) {
    char inChar = Serial.read();
    if (inChar == '\r') continue;
    if (inChar == '\n') {
      inputBuffer.trim();
      if (inputBuffer.equalsIgnoreCase("WIFI")) {
        ssid = "";
        password = "";
        promptUserForWiFi(ssid, password);
        saveWiFiConfig(ssid, password);
        Serial.println("WiFi data updated!");
        tryConnectWiFi();
      }
      else if (inputBuffer.equalsIgnoreCase("TIME")) {
        readUARTTime();
      }
      inputBuffer = "";
    } else {
      inputBuffer += inChar;
    }
  }
}

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
  connectToWiFi();
  setTime();
  Wire.begin(def_SDA, def_SCL);
  initSensors();
  initServer();
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  Serial.println("If you want to change WiFi data use command: WIFI ");
  delay(2000);
}
void loop() {
  useCommandForChanging();
  Serial.println("Start loop");
  print_sensors_data(get_sensors_data());
  server.handleClient();
  Serial.println("════════════════════════════════");
  Serial.println("CONNECT TO SATELLITE CONSOLE AT: ");
  Serial.println(WiFi.localIP());
  Serial.println("════════════════════════════════\n\n\n");
   
  delay(1000);
}
