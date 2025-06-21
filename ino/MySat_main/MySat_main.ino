// MySatKit-Firmware V.1.0.1
// Release date: 2025/05/13

#define FIRMWARE_VERSION "v.1.0.5"
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

void saveWiFiConfig(const String& ssid, const String& password, const String& useWiFi) {  //write data to a file
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

void promptUserForWiFi(String& ssid, String& password) {  //receive data from the user
  unsigned long startTime = millis();
  unsigned long lastRepeat = startTime;

  Serial.print("Please enter WiFi ssid: ");
  while (ssid.length() == 0) {
    if (Serial.available()) {
      ssid = Serial.readStringUntil('\n');
      ssid.trim();
      Serial.println(ssid);
      break;
    }

    if (millis() - lastRepeat >= 10000) {
      Serial.print("\nPlease enter WiFi ssid: ");
      lastRepeat = millis();
    }
    delay(50);
  }

  startTime = millis();
  lastRepeat = startTime;

  Serial.print("Please enter WiFi password: ");
  while (password.length() == 0) {
    if (Serial.available()) {
      password = Serial.readStringUntil('\n');
      password.trim();
      Serial.println(password);
      break;
    }

    if (millis() - lastRepeat >= 10000) {
      Serial.print("\nPlease enter WiFi password: ");
      lastRepeat = millis();
    }
    delay(50);
  }
}

void tryConnectWiFi() {
  while (true) {
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    int retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 20) {
      delay(500);
      Serial.print(".");
      retry++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi connected successfully!");
      return;
    } else {
      Serial.println("\nFailed to connect!");
      setWiFi();
      if (!useWiFi.equalsIgnoreCase("Yes")) {
        return;
      }
    }
  }
}

void setWiFi() {
  unsigned long startTime = millis();
  unsigned long lastRepeat = startTime;

  Serial.println("Do you want to use WiFi? Yes/No");
  while (!Serial.available()) {
    if (millis() - lastRepeat >= 5000) {
      Serial.println("Do you want to use WiFi? Yes/No");
      lastRepeat = millis();
    }
    delay(50);
  }
  useWiFi = Serial.readStringUntil('\n');
  useWiFi.trim();

  if (useWiFi.equalsIgnoreCase("Yes")) {
    ssid = "";
    password = "";
    promptUserForWiFi(ssid, password);

    if (ssid.length() > 0 && password.length() > 0) {
      saveWiFiConfig(ssid, password, useWiFi);
    } else {
      Serial.println("SSID or password is empty. Config not saved.");
    }

  } else {
    saveWiFiConfig("none", "none", useWiFi);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.println("WiFi disabled by user.");
  }
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
  } else {
    Serial.println("WiFi disabled by user.");
  }
}

void useCommandForChanging() {  // read commands for changing data
  static String inputBuffer = "";

  while (Serial.available() > 0) {
    char inChar = Serial.read();
    if (inChar == '\r') continue;
    if (inChar == '\n') {
      inputBuffer.trim();
      if (inputBuffer.equalsIgnoreCase("ChangeTime")) {
        readUARTTime();

      } else if (inputBuffer.equalsIgnoreCase("SetWIFI")) {
        setWiFi();
        tryConnectWiFi();

      } else if (inputBuffer.equalsIgnoreCase("TurnLed")) {
        light_on();

      } else if (inputBuffer.equalsIgnoreCase("SolarDeploy")) {
        stateMotor = true;
        control_motor(stateMotor);

      } else if (inputBuffer.equalsIgnoreCase("SolarRetract")) {
        stateMotor = false;
        control_motor(stateMotor);
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
  initSignalLed();
  initStarLed();
  setTime();
  Wire.begin(def_SDA, def_SCL);
  initSensors();
  if (useWiFi.equalsIgnoreCase("Yes")) {
    initServer();
  }
  Serial.println("If you want to change WiFi data use the command: SetWIFI ");
  delay(2000);
}

unsigned long lastSystemCheck = 0;

void loop() {
  useCommandForChanging();
  Serial.println("\n\n\nStart loop (" FIRMWARE_VERSION ")");
  pointer_of_sensors* data = get_sensors_data();
  get_all_sensor_data(data, stateMotor);  
  print_sensors_data(data, stateMotor);
  Serial.println(stateMotor ? "Solar panels deployed" : "Solar panels retracted");
  Serial.println("════════════════════════════════");
  Serial.print("StarLED status: ");
  Serial.println(stateLight ? "ON" : "OFF");
  if (millis() - lastSystemCheck > 5000) {
    evaluateSystemState();
    lastSystemCheck = millis();
  }
  updateSignalLed();

  if (useWiFi.equalsIgnoreCase("Yes")) {
    server.handleClient();
    Serial.println("════════════════════════════════");
    Serial.println("CONNECT TO SATELLITE CONSOLE AT: ");
    Serial.println(WiFi.localIP());
    Serial.println("════════════════════════════════");
  } else {
    Serial.println("════════════════════════════════");
    Serial.println("WiFi not configured. Use the command: SetWIFI");
  }

  delay(1000);
}
