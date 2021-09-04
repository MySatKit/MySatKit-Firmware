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
#include <RtcDS3231.h>
#include <Wire.h>
// RTC
#include "RTC.h"
// MPU9250
#include <MPU9250_asukiaaa.h> // http://www.esp32learning.com/code/esp32-and-mpu-9250-sensor-example.php
#include "MPU9250.h"

#include "Adafruit_INA219.h" //https://github.com/Makuna/Rtc
#include <Adafruit_BME280.h>
#include "log.h"

//Adafruit_INA219 ina219;
#include "INA219.h"
float temperature;
float humidity;
float pressure;

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
int ph1,ph2,ph3,ph4;
// Replace with your network credentials
//const char* ssid = "LUNAR_WIFI";
//const char* password = "ElonMars2024?";
const char* ssid = "Tenda_C000C0";
const char* password = "12345678";

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

String processor(const String& var)
{
  getSensorReadings(temperature, humidity, pressure);
  Log::printDateTimeS(rtcGetTime());
  Log::printValuesBME(getBME280());
  Log::printValuesMPU9250(getMPU9250());
  Log::printValuesINA219(getINA219());
  Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(temperature);
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
  return String("No data");
}

// Create AsyncWebServer object on port 80
boolean isPhotoNeeded = false;
#include "AsyncWebServer.h"

void connectWiFI()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
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

void setup() 
{
  pinMode(10, INPUT);    // sets the digital pin 13 as output
  Serial.begin(115200);
  delay(100);
  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.println("");
  Serial.println("Start Init");

  i2cInit();  // Init I2C
  connectWiFI();
  initSPIFFS();
  initCamera();
  initServer();
  initBME();
  initMPU9250();
  initINA219();
  initRTC();
  digitalWrite(10, LOW);
  Log::printWiFiInfo();
}

void loop() 
{
  digitalWrite(10, LOW);
  takePhoto();
  getSensorReadings(temperature, humidity, pressure);
  sendEvents();// Send Events to the Web Server with the Sensor Readings
  Serial.println("");
  Serial.println("Loop Start");  
  Serial.println("");
  Log::printDateTimeS(rtcGetTime());
  Serial.println();
  rtcGetTemperature().Print(Serial);
  Serial.println("C");
  Log::printValuesBME(getBME280());
  Log::printValuesMPU9250(getMPU9250());
  Log::printValuesINA219(getINA219());
  Log::printWiFiInfo();
  Serial.println("Loop End");  
  Serial.println("");
  delay(1000); 
}
