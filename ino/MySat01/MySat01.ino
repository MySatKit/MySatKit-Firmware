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

#include <Wire.h>
#include <MPU9250_asukiaaa.h> // http://www.esp32learning.com/code/esp32-and-mpu-9250-sensor-example.php

// RTC
#include <RtcDS3231.h>
#include "rtc.h"
#include "pressure.h"
#include "mcu.h"

#include "Adafruit_INA219.h"
#include <Adafruit_BME280.h>


Adafruit_INA219 ina219;
MPU9250_asukiaaa mySensor(0x69);
Adafruit_BME280 bme;
float temperature;
float humidity;
float pressure;



float shuntvoltage = 0;
float busvoltage = 0;
float current = 0;
float power = 0;

float loadvoltage = 0;

// I2C
#define I2C_SDA 2
#define I2C_SCL 4
TwoWire I2Cnew = TwoWire(0);

// RTC
RtcDS3231<TwoWire> Rtc(I2Cnew);
RtcDateTime now;
RtcTemperature temp;
String sTime;

float aX, aY, aZ, aSqrt, gX, gY, gZ, mDirection, mX, mY, mZ;
#define SEALEVELPRESSURE_HPA (1013.25)
int ph1,ph2,ph3,ph4;
// Replace with your network credentials
const char* ssid = "TP-LINK_87CC";
const char* password = "Kelep1632";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Create an Event Source on /events
AsyncEventSource events("/events"); //https://randomnerdtutorials.com/esp32-web-server-sent-events-sse/

boolean takeNewPhoto = false;
// Photo File Name to save in SPIFFS
//https://randomnerdtutorials.com/esp32-cam-take-photo-display-web-server/
#define FILE_PHOTO "/photo.jpg" 


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

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>MySat UA</title>
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p { font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #50B8B4; color: white; font-size: 1rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 800px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .cards2 { max-width: 800px; margin: 0 auto; display: grid; grid-gap: 3rem; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .reading { font-size: 1.4rem; }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>MySat UA Mission Control</h1>
  </div>
  <div id="container">
    <h2>Picture from MySat</h2>
    <p>It might take more than 5 seconds to capture a photo.</p>
    <p>
      <button onclick="capturePhoto()">CAPTURE PHOTO</button>
      <button onclick="location.reload();">REFRESH PAGE</button>
    </p>
  </div>
  <div>
      <img src="saved-photo" id="photo"></img>  
  </div>
  <div class="content">
    <div class="cards">
      <div class="card">
        <p><i class="fas fa-thermometer-half" style="color:#059e8a;"></i> TEMPERATURE</p><p><span class="reading"><span id="temp">%TEMPERATURE%</span> &deg;C</span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-tint" style="color:#00add6;"></i> HUMIDITY</p><p><span class="reading"><span id="hum">%HUMIDITY%</span> &percnt;</span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-angle-double-down" style="color:#e1e437;"></i> PRESSURE</p><p><span class="reading"><span id="pres">%PRESSURE%</span> hPa</span></p>
      </div>
    </div>
    <div class="cards">
      <div class="card">
        <p><i class="far fa-clock" style="color:#00add6;"></i> TIME</p><p><span class="reading"><span id="time_id">%TIME%</span> &percnt;</span></p>
      </div>
    </div>
    <div class="cards">
      <div class="card">
        <p><i class="fas fa-arrow-right" style="color:#059e8a;"></i> A X</p><p><span class="reading"><span id="ax_id">%AX%</span> </span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-arrow-down" style="color:#00add6;"></i> A Y</p><p><span class="reading"><span id="ay_id">%AY%</span> </span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-arrow-up" style="color:#e1e437;"></i> A Z</p><p><span class="reading"><span id="az_id">%AZ%</span> </span></p>
      </div>
    </div>
    <div class="cards">
      <div class="card">
        <p><i class="fas fa-arrow-right" style="color:#059e8a;"></i> G X</p><p><span class="reading"><span id="gx_id">%GX%</span> </span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-arrow-down" style="color:#00add6;"></i> G Y</p><p><span class="reading"><span id="gy_id">%GY%</span> </span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-arrow-up" style="color:#e1e437;"></i> G Z</p><p><span class="reading"><span id="gz_id">%GZ%</span> </span></p>
      </div>
    </div>
    <div class="cards">
      <div class="card">
        <p><i class="fas fa-arrow-right" style="color:#059e8a;"></i> M X</p><p><span class="reading"><span id="mx_id">%MX%</span> </span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-arrow-down" style="color:#00add6;"></i> M Y</p><p><span class="reading"><span id="my_id">%MY%</span> </span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-arrow-up" style="color:#e1e437;"></i> M Z</p><p><span class="reading"><span id="mz_id">%MZ%</span> </span></p>
      </div>
    </div>
    <div class="cards2">
      <div class="card">
        <p><i class="fas fa-sun" style="color:#059e8a;"></i> Ph1</p><p><span class="reading"><span id="ph1_id">%PH1%</span> </span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-sun" style="color:#00add6;"></i> Ph2</p><p><span class="reading"><span id="ph2_id">%PH2%</span> </span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-sun" style="color:#e1e437;"></i> Ph3</p><p><span class="reading"><span id="ph3_id">%PH3%</span> </span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-sun" style="color:#e1e437;"></i> Ph4</p><p><span class="reading"><span id="ph4_id">%PH4%</span> </span></p>
      </div>
    </div>
    <div class="cards2">
      <div class="card">
        <p><i class="fas fa-sun" style="color:#059e8a;"></i> shuntvoltage</p><p><span class="reading"><span id="shuntvoltage_id">%SHUNTVOLTAGE%</span> V</span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-sun" style="color:#00add6;"></i> busvoltage</p><p><span class="reading"><span id="busvoltage_id">%BUSVOLTAGE%</span> V</span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-sun" style="color:#e1e437;"></i> current</p><p><span class="reading"><span id="current_id">%CURRENT%</span> mA</span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-sun" style="color:#e1e437;"></i> power</p><p><span class="reading"><span id="power_id">%POWER%</span> mW</span></p>
      </div>
    </div>
  </div> 
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
  source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);
 source.addEventListener('temperature', function(e) {
  console.log("temperature", e.data);
  document.getElementById("temp").innerHTML = e.data;
 }, false);
 source.addEventListener('humidity', function(e) {
  console.log("humidity", e.data);
  document.getElementById("hum").innerHTML = e.data;
 }, false);
 source.addEventListener('pressure', function(e) {
  console.log("pressure", e.data);
  document.getElementById("pres").innerHTML = e.data;
 }, false);
 source.addEventListener('sTime', function(e) {
  console.log("sTime", e.data);
  document.getElementById("time_id").innerHTML = e.data;
 }, false);
 source.addEventListener('aX', function(e) {
  console.log("aX", e.data);
  document.getElementById("ax_id").innerHTML = e.data;
 }, false);
 source.addEventListener('aY', function(e) {
  console.log("aY", e.data);
  document.getElementById("ay_id").innerHTML = e.data;
 }, false);
 source.addEventListener('aZ', function(e) {
  console.log("aZ", e.data);
  document.getElementById("az_id").innerHTML = e.data;
 }, false);
 source.addEventListener('gX', function(e) {
  console.log("gX", e.data);
  document.getElementById("gx_id").innerHTML = e.data;
 }, false);
 source.addEventListener('gY', function(e) {
  console.log("gY", e.data);
  document.getElementById("gy_id").innerHTML = e.data;
 }, false);
 source.addEventListener('gZ', function(e) {
  console.log("gZ", e.data);
  document.getElementById("gz_id").innerHTML = e.data;
 }, false);
 source.addEventListener('mX', function(e) {
  console.log("mX", e.data);
  document.getElementById("mx_id").innerHTML = e.data;
 }, false);
 source.addEventListener('mY', function(e) {
  console.log("mY", e.data);
  document.getElementById("my_id").innerHTML = e.data;
 }, false);
 source.addEventListener('mZ', function(e) {
  console.log("mZ", e.data);
  document.getElementById("mz_id").innerHTML = e.data;
 }, false);
 source.addEventListener('ph1', function(e) {
  console.log("ph1", e.data);
  document.getElementById("ph1_id").innerHTML = e.data;
 }, false);
 source.addEventListener('ph2', function(e) {
  console.log("ph2", e.data);
  document.getElementById("ph2_id").innerHTML = e.data;
 }, false);
 source.addEventListener('ph3', function(e) {
  console.log("ph3", e.data);
  document.getElementById("ph3_id").innerHTML = e.data;
 }, false);
 source.addEventListener('ph4', function(e) {
  console.log("ph4", e.data);
  document.getElementById("ph4_id").innerHTML = e.data;
 }, false);
 source.addEventListener('shuntvoltage', function(e) {
  console.log("shuntvoltage", e.data);
  document.getElementById("shuntvoltage_id").innerHTML = e.data;
 }, false);
 source.addEventListener('busvoltage', function(e) {
  console.log("busvoltage", e.data);
  document.getElementById("busvoltage_id").innerHTML = e.data;
 }, false);
 source.addEventListener('current', function(e) {
  console.log("current", e.data);
  document.getElementById("current_id").innerHTML = e.data;
 }, false);
 source.addEventListener('power', function(e) {
  console.log("power", e.data);
  document.getElementById("power_id").innerHTML = e.data;
 }, false);
 function capturePhoto() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/capture", true);
    xhr.send();
 }
}
</script>
</body>
</html>
)rawliteral";

void getSensorReadings(){
  temperature = bme.readTemperature();
  // Convert temperature to Fahrenheit
  //temperature = 1.8 * bme.readTemperature() + 32;
  humidity = bme.readHumidity();
  pressure = bme.readPressure()/ 100.0F;
}



void printValuesBME() {
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
}





void printValuesINA219()
{
  

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current = ina219.getCurrent_mA();
  power = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

  Serial.print("BV"); Serial.print("\t"); // Bus Voltage
  Serial.print("SV"); Serial.print("\t"); // Shunt Voltage
  Serial.print("LV"); Serial.print("\t"); // Load Voltage
  Serial.print("C"); Serial.print("\t");  // Current
  Serial.println("P");  // Power

  Serial.print(busvoltage); Serial.print("\t"); 
  Serial.print(shuntvoltage); Serial.print("\t");
  Serial.print(loadvoltage); Serial.print("\t");
  Serial.print(current); Serial.print("\t");
  Serial.println(power);

}


void printValues9250() {
    uint8_t sensorId = 0x69;
  if (mySensor.readId(&sensorId) == 0) {
    Serial.println("sensorId: " + String(sensorId));
  } else {
    Serial.println("Cannot read sensorId");
  }
 
  if (mySensor.accelUpdate() == 0) {
    aX = mySensor.accelX();
    aY = mySensor.accelY();
    aZ = mySensor.accelZ();
    aSqrt = mySensor.accelSqrt();
    Serial.println("accelX: " + String(aX));
    Serial.println("accelY: " + String(aY));
    Serial.println("accelZ: " + String(aZ));
    Serial.println("accelSqrt: " + String(aSqrt));
  } else {
    Serial.println("Cannod read accel values");
  }
 
  if (mySensor.gyroUpdate() == 0) {
    gX = mySensor.gyroX();
    gY = mySensor.gyroY();
    gZ = mySensor.gyroZ();
    Serial.println("gyroX: " + String(gX));
    Serial.println("gyroY: " + String(gY));
    Serial.println("gyroZ: " + String(gZ));
  } else {
    Serial.println("Cannot read gyro values");
  }
 
  if (mySensor.magUpdate() == 0) {
    mX = mySensor.magX();
    mY = mySensor.magY();
    mZ = mySensor.magZ();
    mDirection = mySensor.magHorizDirection();
    Serial.println("magX: " + String(mX));
    Serial.println("maxY: " + String(mY));
    Serial.println("magZ: " + String(mZ));
    Serial.println("horizontal direction: " + String(mDirection));
  } else {
    Serial.println("Cannot read mag values");
  }

}



void printValuesPhotoRes(){

Serial.println("Phores"); 

 Serial.print("Photo 1: ");
// ph1 = analogRead(12);
 Serial.println(ph1);

 Serial.print("Photo 2: ");
// ph2 = analogRead(13);
 Serial.println(ph2);

 Serial.print("Photo 3: ");
// ph3 = analogRead(14);
 Serial.println(ph3);

 Serial.print("Photo 4: ");
// ph4 = analogRead(15);
 Serial.println(ph4);

}

String processor(const String& var){
  getSensorReadings();
  now = Rtc.GetDateTime();
  printDateTimeS(now);
  printValuesBME();
  printValues9250();
  printValuesINA219();
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



// Check if photo capture was successful
bool checkPhoto( fs::FS &fs ) {
  Serial.println("checkPhoto");
  File f_pic = fs.open( FILE_PHOTO );
  unsigned int pic_sz = f_pic.size();
  return ( pic_sz > 100 );
}




// Capture Photo and Save it to SPIFFS
void capturePhotoSaveSpiffs( void ) {
  Serial.println("capturePhotoSaveSpiffs");
  camera_fb_t * fb = NULL; // pointer
  bool ok = 0; // Boolean indicating if the picture has been taken correctly

  do {
    // Take a photo with the camera
    Serial.println("Taking a photo...");

    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    }

    // Photo file name
    Serial.printf("Picture file name: %s\n", FILE_PHOTO);
    File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);

    // Insert the data in the photo file
    if (!file) {
      Serial.println("Failed to open file in writing mode");
    }
    else {
      file.write(fb->buf, fb->len); // payload (image), payload length
      Serial.print("The picture has been saved in ");
      Serial.print(FILE_PHOTO);
      Serial.print(" - Size: ");
      Serial.print(file.size());
      Serial.println(" bytes");
    }
    // Close the file
    file.close();
   esp_camera_fb_return(fb);

   //  check if file has been correctly saved in SPIFFS
        ok = checkPhoto(SPIFFS);
  } while ( !ok );
}
















void blinkLEDs(){
// Blink leds
digitalWrite(16, HIGH); 
digitalWrite(0, HIGH); 
delay(1000); 
digitalWrite(16, LOW); 
digitalWrite(0, LOW); 
delay(1000); 
  
}





void setup() {

  Serial.begin(115200);
    delay(100);


  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);


   Serial.println("");
   Serial.println("Start Init");
 
   I2Cnew.begin(I2C_SDA, I2C_SCL, 400000);









  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Print ESP32 Local IP Address
  Serial.print("IP Address: http://");
  Serial.println(WiFi.localIP());

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("SPIFFS mounted successfully");
  }
 
 

  // OV2640 camera module
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  }
  else
  {
    Serial.println("Camera init... OK");
  }

  server.on("/capture", HTTP_GET, [](AsyncWebServerRequest * request) {
    takeNewPhoto = true;
    request->send_P(200, "text/plain", "Taking Photo");
  });

  server.on("/saved-photo", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, FILE_PHOTO, "image/jpg", false);
  });

  // Start server
  //server.begin();


  // Handle Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Handle Web Server Events
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  
  server.addHandler(&events);
  server.begin();



    bool status;
    // BME
    Serial.print("Check BME...");
    status = bme.begin(0x76, &I2Cnew);  
    if (status)
    {
        Serial.println(" Done.");
    } 
    else
    {
        Serial.println(" Fail");
    }

  // 9250
  Serial.print("Check MCU...");
  mySensor.setWire(&I2Cnew);
  mySensor.beginAccel();
  mySensor.beginGyro();
  mySensor.beginMag();
  Serial.println(" TBD.");
  
  Serial.print("Check RTC...");
  Serial.println(" TBD.");

    Serial.print("Check INA219...");
    status = ina219.begin(&I2Cnew);  
    if (status)
    {
        Serial.println(" Done.");
    } 
    else
    {
        Serial.println(" Fail");
    }

// LED (Boot)
//pinMode(16, OUTPUT);

// LED (LED Star)
//pinMode(0, OUTPUT);


// Photores
// pinMode(12, INPUT);
// pinMode(13, INPUT);
// pinMode(14, INPUT);
// pinMode(15, INPUT);


// RTC
    Rtc.Begin();





  Serial.println("End Init");
  Serial.println("");

}







void loop() {
 Serial.println("");
 Serial.println("Loop begin");

/*  if (takeNewPhoto) {
    Serial.println("takeNewPhoto");
    capturePhotoSaveSpiffs();
    takeNewPhoto = false;
  }
  
*/ 
  now = Rtc.GetDateTime();
  printDateTimeS(now);
  Serial.println();

  temp = Rtc.GetTemperature();
  temp.Print(Serial); // you may also get the temperature as a float and print it Serial.print(temp.AsFloatDegC());
  Serial.println("C");

   printValuesBME();
   printValues9250();
   printValuesINA219();
 
   //printValuesPhotoRes();
  // blinkLEDs();






    getSensorReadings();
   Serial.printf("Temperature = %.2f ºC \n", temperature);
    Serial.printf("Humidity = %.2f \n", humidity);
    Serial.printf("Pressure = %.2f hPa \n", pressure);
    Serial.printf("Time: ", sTime);
    Serial.println();

    // Send Events to the Web Server with the Sensor Readings
    events.send("ping",NULL,millis());
    events.send(String(temperature).c_str(),"temperature",millis());
    Serial.print("Send event: ");
    Serial.println(String(temperature).c_str());
 
    events.send(String(humidity).c_str(),"humidity",millis());
    events.send(String(pressure).c_str(),"pressure",millis());
    events.send(String(sTime).c_str(),"sTime",millis());
 
    events.send(String(aX).c_str(),"ax_id",millis());
    events.send(String(aY).c_str(),"ay_id",millis());
    events.send(String(aZ).c_str(),"az_id",millis());

    events.send(String(gX).c_str(),"gx_id",millis());
    events.send(String(gY).c_str(),"gy_id",millis());
    events.send(String(gZ).c_str(),"gz_id",millis());

    events.send(String(mX).c_str(),"mx_id",millis());
    events.send(String(mY).c_str(),"my_id",millis());
    events.send(String(mZ).c_str(),"mz_id",millis());

    events.send(String(ph1).c_str(),"ph1_id",millis());
    events.send(String(ph2).c_str(),"ph2_id",millis());
    events.send(String(ph3).c_str(),"ph3_id",millis());
    events.send(String(ph4).c_str(),"ph4_id",millis());
 
    events.send(String(shuntvoltage).c_str(),"shuntvoltage_id",millis());
    events.send(String(busvoltage).c_str(),"busvoltage_id",millis());
    events.send(String(current).c_str(),"current_id",millis());
    events.send(String(power).c_str(),"power_id",millis());
 




 now = Rtc.GetDateTime();
 printDateTimeS(now);
 Serial.println();
  
 Serial.println("Loop End");  // Power
 Serial.println("");

 
 
delay(1000); 
}
