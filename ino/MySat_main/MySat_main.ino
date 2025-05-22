// MySatKit-Firmware V.1.0.1
// Release date: 2025/05/13

#include <Wire.h>
#include "server.h"
#include <SPIFFS.h>

String ssid = "";
String password = "";
String useWiFi = "";

bool loadWiFiConfig(String& ssid, String& password, String& useWiFi) {
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS mount failed!");
    return false;
  }

  File file = SPIFFS.open("/config.txt", "r");
  if (!file) {
    Serial.println("No config.txt found");
    return false;
  }

  int lineCount = 0;
  while (file.available()) {
    file.readStringUntil('\n');
    lineCount++;
  }
  file.close();

  if (lineCount < 3) {
    Serial.println("Invalid config.txt: less than 3 lines");
    SPIFFS.remove("/config.txt");
    return false;
  }

  file = SPIFFS.open("/config.txt", "r");
  useWiFi = file.readStringUntil('\n');
  useWiFi.trim();
  ssid = file.readStringUntil('\n');
  ssid.trim();
  password = file.readStringUntil('\n');
  password.trim();
  file.close();

  if (useWiFi.length() == 0 || ssid.length() == 0 || password.length() == 0) {
    Serial.println("Invalid config: empty fields");
    return false;
  }

  return true;
}

void saveWiFiConfig(const String& ssid, const String& password, const String& useWiFi) {  //записуємо дані в файл
  File file = SPIFFS.open("/config.txt", "w");
  if (!file) {
    Serial.println("Can`t write config.txt");
    return;
  }
  file.println(useWiFi);
  file.println(ssid);
  file.println(password);
  file.close();
}

void promptUserForWiFi(String& ssid, String& password) {  //отримуємо від користувача дані
  unsigned long startTime = millis();
  unsigned long lastRepeat = startTime;

  Serial.println("Please enter WiFi ssid: ");
  while (ssid.length() == 0) {
    if (Serial.available()) {
      ssid = Serial.readStringUntil('\n');
      ssid.trim();
      Serial.println("Entered ssid: " + ssid);
      break;
    }

    if (millis() - lastRepeat >= 10000) {
      Serial.println("Please enter WiFi ssid: ");
      lastRepeat = millis();
    }
    delay(50);
  }

  startTime = millis();
  lastRepeat = startTime;

  Serial.println("Please enter WiFi password: ");
  while (password.length() == 0) {
    if (Serial.available()) {
      password = Serial.readStringUntil('\n');
      password.trim();
      Serial.println("Entered password: " + password);
      break;
    }

    if (millis() - lastRepeat >= 10000) {
      Serial.println("Please enter WiFi password: ");
      lastRepeat = millis();
    }
    delay(50);
  }
}


void tryConnectWiFi() {
  WiFi.begin(ssid.c_str(), password.c_str());

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect! Please enter a new data:");
    ssid = "";
    password = "";
    promptUserForWiFi(ssid, password);
    saveWiFiConfig(ssid, password, useWiFi);

    WiFi.begin(ssid.c_str(), password.c_str());
    retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 20) {
      delay(500);
      Serial.print(".");
      retry++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("WiFi connected successfully!");
    } else {
      Serial.println("Please check and change your WiFi data!!!");
    }
  } else {
    Serial.println("Connected successfully!");
  }
}

void setWiFi() {
  unsigned long startTime = millis();
  unsigned long lastRepeat = startTime;

  Serial.println("Do you want to use WiFi? Yes/No");
  while (!Serial.available()) {
    if (millis() - lastRepeat >= 10000) {                   //використовуємо millis() щоб рахувати секунди для введення 
    Serial.println("Do you want to use WiFi? Yes/No");      // і виведення повідомлення знову,якщо лишаємось без відповіді
    lastRepeat = millis();
    }
    delay(50);
  }
  useWiFi = Serial.readStringUntil('\n');
  useWiFi.trim();

  if (useWiFi.equalsIgnoreCase("Yes")) {
    promptUserForWiFi(ssid, password);
  }
  saveWiFiConfig(ssid, password, useWiFi);
}

void connectToWiFi() {
  if (!loadWiFiConfig(ssid, password, useWiFi)) {
    Serial.println("No WiFi config. Enter data for WiFi");
    setWiFi();
  }
  if (useWiFi.equalsIgnoreCase("Yes")) {
    Serial.print("Connecting to WiFi: ");
    Serial.print(ssid + " ...");

    tryConnectWiFi();
  }else{
    Serial.println("WiFi disabled by user.");
  }
}

void useCommandForChanging() {  // читаємо команди для зміни даних
  static String inputBuffer = "";

  while (Serial.available() > 0) {
    char inChar = Serial.read();
    if (inChar == '\r') continue;
    if (inChar == '\n') {
      inputBuffer.trim();
      if (inputBuffer.equalsIgnoreCase("ChangeWIFI")) {
        ssid = "";
        password = "";
        promptUserForWiFi(ssid, password);
        saveWiFiConfig(ssid, password, useWiFi);
        Serial.println("WiFi data updated!");
        tryConnectWiFi();

      } else if (inputBuffer.equalsIgnoreCase("Time")) {
        readUARTTime();

      } else if (inputBuffer.equalsIgnoreCase("SetWIFI")) {
        setWiFi();
        tryConnectWiFi();
      }
      inputBuffer = "";
    } else {
      inputBuffer += inChar;
    }
  }
}


const int def_SDA(15);
const int def_SCL(13);

struct sensors_structure {
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

  if (useWiFi.equalsIgnoreCase("Yes")) {
    server.handleClient();
    Serial.println("════════════════════════════════");
    Serial.println("CONNECT TO SATELLITE CONSOLE AT: ");
    Serial.println(WiFi.localIP());
    Serial.println("════════════════════════════════\n\n\n");
  } else {
    Serial.println("════════════════════════════════");
    Serial.println("WiFi not configured. Use command: SETWIFI");
  }

  delay(1000);
}
