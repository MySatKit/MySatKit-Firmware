#define CAMERA_MODEL_AI_THINKER
#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#ifdef CORE_DEBUG_LEVEL
#undef CORE_DEBUG_LEVEL
#endif

#define CORE_DEBUG_LEVEL 3
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

#include "WiFi.h"

#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "soc/soc.h"           // Disable brownour problems
#include "soc/rtc_cntl_reg.h"  // Disable brownour problems
#include "driver/rtc_io.h"
#include <ESPAsyncWebServer.h>
#include <StringArray.h>
#include <SPIFFS.h>
#include <FS.h>

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"

// RTC
#include "RTC.h"
// MPU9250
#include "MPU9250.h"
//Adafruit_INA219 ina219;
#include "INA219.h"
#include "log.h"
#include "ADS.h"
//#include <analogWrite.h>

float temperature;
float humidity;
float pressure;

#include <ESP32Servo.h>

// File system with html and css
// https://randomnerdtutorials.com/esp32-web-server-spiffs-spi-flash-file-system/

float shuntvoltage = 0;
float busvoltage = 0;
float current = 0;
float power = 0;

// I2C
#include "I2C.h"

#include "BME280.h"
String sTime;
float aX, aY, aZ, aSqrt, gX, gY, gZ, mDirection, mX, mY, mZ;
#define SEALEVELPRESSURE_HPA (1013.25)
float ph1,ph2,ph3,ph4;
int numPosition;
// Replace with your network credentials
//const char* ssid = "LUNAR_WIFI";
//const char* password = "ElonMars2024";
const char* ssid = "Lunar2024";
const char* password = "ElonMars2@";

bool flag = true;

String inString;
int led = 14;

String commandToLightLED = "";

String nameProbe = "";

int potpin = 0;
int vall;

// Photo File Name to save in SPIFFS
//https://randomnerdtutorials.com/esp32-cam-take-photo-display-web-server/
#define FILE_PHOTO "/photo.jpg" 
#include "Camera.h"
// OV2640 camera module pins (CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// star pin
#define STAR_GIPIO 16
File myFile;
String processor(const String& var)
{
  getSensorReadings(temperature, humidity, pressure);
  Log::printDateTimeS(rtcGetTime());
  Log::printValuesBME(getBME280());
  Log::printValuesADS(getADS1015());
  Log::printValuesMPU9250(getMPU9250());
  Log::printValuesINA219(getINA219());
  Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(temperature);
  }
  else if(var == "SUN_POSITION"){
    return String(numPosition);
    }
   else if(var == "WIFI_RSSI"){
    return String(WiFi.RSSI());
   }
  else if(var == "NAME"){
    return nameProbe;
  }
  else if(var == "HUMIDITY"){
    return String(humidity);
  }
  else if(var == "PRESSURE"){
    return String(pressure);
  }
  else if(var == "TIME"){
    return String(sTime);
  }
  else if(var == "AX"){
    return String(aX);
  }
  else if(var == "AY"){
    return String(aY);
  }
  else if(var == "AZ"){
    return String(aZ);
  }
  else if(var == "GX"){
    return String(gX);
  }
  else if(var == "GY"){
    return String(gY);
  }
  else if(var == "GZ"){
    return String(gZ);
  }
  else if(var == "MX"){
    return String(mX);
  }
  else if(var == "MY"){
    return String(mY);
  }
  else if(var == "MZ"){
    return String(mZ);
  }
   else if(var == "PH1"){
    return String(ph1);
  }
   else if(var == "PH2"){
    return String(ph2);
  }
   else if(var == "PH3"){
    return String(ph3);
  }
   else if(var == "PH4"){
    return String(ph4);
  }
   else if(var == "SHUNTVOLTAGE"){
    return String(shuntvoltage);
  }
   else if(var == "BUSVOLTAGE"){
    return String(busvoltage);
  }
   else if(var == "CURRENT"){
    return String(current);
  }
   else if(var == "POWER"){
    return String(power);
  }
  else if(var == "ONBOARD_TIME"){
    return String(rtcGetTime());
  }
  return String("No data");
}

// Create AsyncWebServer object on port 80
boolean isPhotoNeeded = false;
boolean isLedLight = false;
#include "AsyncWebServer.h"

void connectWiFI()
{
  String logn, pas;
  readConfig(SPIFFS, logn, pas);
  WiFi.begin(logn.c_str(), pas.c_str());
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    Serial.println(logn);
    Serial.println(pas);
    readUART();
  }
}

void initSPIFFS()
{
  if (!SPIFFS.begin(true)) 
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else 
  {
    delay(500);
    Serial.println("SPIFFS mounted successfully");
  }
}

void takePhoto()
{
  if (isPhotoNeeded) 
  {
    capturePhotoSaveSpiffs();
    isPhotoNeeded = false;
  }
}


/*
void switchStar()
{
    if(!isLedLight)
    //analogWrite(STAR_GIPIO, 0);
  else
    //analogWrite(STAR_GIPIO, 255);
    Serial.println("else");
}*/

int sunPosition(){
  int coef;
  if (ph1 < ph2 && ph1 < ph3 && ph1 < ph4){ 
    if (ph2 < ph4){
        coef = ((ph2 - ph1)/ ph2) * 100;
        if (coef < 10){
          return 2;
          }
        else{
          return 1;
          }
      }
    else if(ph2 > ph4){
        coef = ((ph4 - ph1)/ ph4) * 100;
        if (coef < 10){
          return 12;
          }
        else{
          return 1;
          }
      }
  } 
  else if(ph2 < ph1 && ph2 < ph3 && ph2 < ph4)
  {
    if (ph1 < ph3){
        coef = ((ph1 - ph2)/ ph1) * 100;
        if (coef < 10){
          return 3;
          }
        else{
          return 4;
          }
      }
    else if(ph1 > ph3){
        coef = ((ph3 - ph2)/ ph3) * 100;
        if (coef < 10){
          return 5;
          }
        else{
          return 4;
          }
      }
  }
  else if(ph3 < ph1 && ph3 < ph2 && ph3 < ph4)
  {
    if (ph2 < ph4){
        coef = ((ph2 - ph3)/ ph2) * 100;
        if (coef < 10){
          return 6;
          }
        else{
          return 7;
          }
      }
    else if(ph2 > ph4){
        coef = ((ph4 - ph3)/ ph4) * 100;
        if (coef < 10){
          return 8;
          }
        else{
          return 7;
          }
      }
  }
  else if(ph4 < ph1 && ph4 < ph2 && ph4 < ph3)
  {
    if (ph1 < ph3){
        coef = ((ph1 - ph4)/ ph1) * 100;
        if (coef < 10){
          return 11;
          }
        else{
          return 10;
          }
      }
    else if(ph1 > ph3){
        coef = ((ph3 - ph4)/ ph3) * 100;
        if (coef < 10){
          return 9;
          }
        else{
          return 10;
          }
      }
  }
}
void writeConfig(fs::FS &fs){
  myFile = fs.open("/config.txt", FILE_WRITE);
  myFile.print("Tenda_C000C0,12345678\n");
  myFile.println(nameProbe);
  myFile.close();
  }

void writeNewConfig(fs::FS &fs, String wifiInf){
  myFile = fs.open("/config.txt", FILE_WRITE);
  myFile.print(wifiInf);
  myFile.print('\n');
  myFile.println(nameProbe);
  myFile.close();
  
  }


void switchStar(String com){
  
  if(com == "ON"){
     digitalWrite(led, HIGH);
  } else if(com == "OFF"){
     digitalWrite(led, LOW);
  }
  if(isLedLight){
     Serial.println("LED ON");
     digitalWrite(led, HIGH);
  } else{
     Serial.println("LED OFF");
     digitalWrite(led, LOW);
  }
}
bool readUART(){
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    inString += (char)inChar;
    if (inChar == '\n') {
      Serial.print("String: ");
      Serial.println(inString);
      if (inString.startsWith("U")){
        String login;
        login = inString.substring(2,inString.length()-1);
        Serial.print("Password, login: ");
        Serial.println(login);
        //writeNewConfig(SPIFFS, login);
        inString = "";
        return false;
      } else if (inString.startsWith("S")){
        String command;
        command = inString.substring(2,inString.length()-1);
        Serial.println("Comanda");
        Serial.println(command);
        switchStar(command);
        inString = "";
        return false;
      }
      else if(inString.startsWith ("T")){
        String setTime;
        setTime = inString.substring(2,inString.length()-1);
        Serial.println("Time:");
        Serial.println(setTime);
        int setYear, setMonth, setDay, setHour, setMinute, setSecond;
        setYear = setTime.substring(0,4).toInt();
        setMonth = setTime.substring(5,7).toInt();
        setDay = setTime.substring(8,10).toInt();
        setHour = setTime.substring(11,13).toInt();
        setMinute = setTime.substring(14,16).toInt();
        setSecond = setTime.substring(17,19).toInt();
        Serial.println("Year:");
        Serial.println(setYear);
        Serial.println("Month:");
        Serial.println(setMonth);
        Serial.println("Dat:");
        Serial.println(setDay);
        Serial.println("Hour:");
        Serial.println(setHour);
        Serial.println("Minute:");
        Serial.println(setMinute);
        Serial.println("Second:");
        Serial.println(setSecond);
        rtcSetTime(setYear, setMonth, setDay, setHour, setMinute, setSecond);
        inString = "";
        }
    }
  }
  }

void readUARTName(String& nameProbe){
  nameProbe = "";
  Serial.print("Hello! And am your personal satellite. Please enter my name: ");
  while(nameProbe == ""){
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    inString += (char)inChar;
    if (inChar == '\n') {
      Serial.print("String: ");
      Serial.println(inString);
      nameProbe = inString.substring(0,inString.length()-1);
    }
  }
  }
}

void writeUARTNameToConfig(fs::FS &fs, String& nameProbe, String text_file){
    int countLine = 1;
    String write_text = "";
    bool flag_write = true;
    for(int i =0; i < text_file.length(); i++){
    if(text_file[i] == '\n'){
      countLine++;
      }
    if(countLine == 2 && flag_write){
     write_text+= "\n";
     write_text+= nameProbe;
     flag_write = false;
      }
    write_text+= text_file[i];
  }
  myFile = myFile = fs.open("/config.txt", FILE_WRITE);
  myFile.print(write_text);
  myFile.close();
}

void readConfigName(fs::FS &fs, String& nameProbe){
  nameProbe = "";
  int countLine = 1;
  myFile = fs.open("/config.txt", FILE_READ);
  String textFile = myFile.readString();
  
  Serial.println(textFile);
  if(!myFile){
    readUARTName(nameProbe);
    Serial.println("NOT MY FILE");
    writeConfig(SPIFFS);
    }
  
  for(int i =0; i < textFile.length(); i++){
      if (textFile[i] == '\n'){
        countLine++;
        }
    if (countLine == 2){
      if(textFile[i]!='\n' && textFile[i] != '\r'){
        nameProbe += textFile[i];
        }
      }
    }
    if (countLine < 2){
      readUARTName(nameProbe);
      Serial.println("NOT 2 LINE");
      }
     if(nameProbe == "" || nameProbe == "\n" || nameProbe == "\r"){
      readUARTName(nameProbe);
      writeUARTNameToConfig(SPIFFS, nameProbe, textFile);
      Serial.println("NOT NAME");
      }
  myFile.close();
  }

void readConfig(fs::FS &fs, String& logn, String&  pas){
   logn = "";
   pas = "";
   bool flag = true;
   int countLine = 1;
   myFile = fs.open("/config.txt", FILE_READ);
   if(!myFile){
    writeConfig(SPIFFS);
    }
   String textFile = myFile.readString();
   
   for(int i = 0; i < textFile.length(); i++){
       if (textFile[i] == '\n'){
          countLine++;
        }
       if (countLine == 1){
       if (textFile[i] == ','){
          flag = false; 
       }else{
           if (flag){
               if (isalnum(textFile[i]) || textFile[i] == '_' || textFile[i] == '.'){
                   logn +=textFile[i];
               }else{
                    writeConfig(SPIFFS);
                    Serial.println("NOT CORRECT PASSWORD");
                }
           }else{
               if(textFile[i] != ' ' && textFile[i]!='\n' && textFile[i]!= '\r'){
                   pas+=textFile[i];
               }
           }
       }
       
   }
   
   }
   if(flag){
    writeConfig(SPIFFS);
    Serial.println("NOT CORRECT FLAG");
    }
   myFile.close();
}
void setup() 
{
  pinMode(STAR_GIPIO, OUTPUT);
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  delay(100);
  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.println("");
  Serial.println("Start Init");
  
  i2cInit();  // Init I2C
  initSPIFFS();
  readConfigName(SPIFFS, nameProbe);
  
  connectWiFI();
  initCamera();
  initServer();
  initBME();
  initADS();
  initMPU9250();
  initINA219();
  initRTC();
  Log::printWiFiInfo();
}

void loop() 
{
  switchStar("");
  takePhoto();
  getSensorReadings(temperature, humidity, pressure);
  getLightSensorReadings(ph1,ph2,ph3,ph4);
  sendEvents();// Send Events to the Web Server with the Sensor Readings
  bool flag = readUART();
  if (flag){
  Serial.println("");
  Serial.println("Loop Start");  
  Serial.println("");
  Serial.print("Your personal satellite: ");
  Serial.println(nameProbe);
  Log::printDateTimeS(rtcGetTime());
  Serial.println();
  rtcGetTemperature().Print(Serial);
  Serial.println("C");
  numPosition = sunPosition();
  
  Log::printValuesBME(getBME280());
  Log::printValuesADS(getADS1015());
  Log::printValuesMPU9250(getMPU9250());
  Log::printValuesINA219(getINA219());
  Log::printWiFiInfo();
  Serial.println("Loop End");
  Serial.println(numPosition);  
  Serial.println("");
  } else{
      Serial.println("Enter new Wifi Name and Password: ");
  }
  delay(1000);
}
