//HC-12 radio module AT-configuration

#pragma once
#include <Wire.h>
#include <Preferences.h>
 
extern Preferences prefs; //shared global instance, declared in MySat_main.ino
 
const uint8_t NANO_CMD_RF_TURN = 2; // must match `enum Commands_list` order on the Nano
const uint8_t NANO_CMD_RF_SET  = 3;
 
const uint32_t HC12_NORMAL_BAUD = 115200;
const uint32_t HC12_FALLBACK_BAUD = 9600;
const unsigned long HC12_RESPONSE_TIMEOUT = 1000; 
const unsigned long HC12_MODE_SWITCH_DELAY = 50; 
const int HC12_DISCONNECT_COUNTDOWN_SEC = 5;
 

 
void hc12EnterConfigMode() { //I2C -> Nano: raise SET pin (D4 = HIGH)
  Wire.beginTransmission(8);
  Wire.write(NANO_CMD_RF_SET);
  Wire.write((uint8_t)1);
  Wire.endTransmission();
  delay(HC12_MODE_SWITCH_DELAY);
}
 
void hc12ExitConfigMode() { //I2C -> Nano: lower SET pin (D4 = LOW), back to radio mode
  Wire.beginTransmission(8);
  Wire.write(NANO_CMD_RF_SET);
  Wire.write((uint8_t)0);
  Wire.endTransmission();
  delay(HC12_MODE_SWITCH_DELAY);
}
 
String hc12WaitForResponse(unsigned long timeout_ms) {
  String response = "";
  unsigned long start = millis();
  while (millis() - start < timeout_ms) {
    while (Serial.available()) {
      response += (char)Serial.read();
    }
    if (response.indexOf("OK") != -1) break; 
  }
  return response;
}
 
String hc12SendATCommand(const String &atCmd, uint32_t baud) {
  hc12EnterConfigMode();
 
  Serial.flush();
  Serial.end();
  Serial.begin(baud);
  delay(20);
 
  Serial.print(atCmd);
  String response = hc12WaitForResponse(HC12_RESPONSE_TIMEOUT);
 
  Serial.flush();
  Serial.end();
  Serial.begin(HC12_NORMAL_BAUD); //always leave the UART back at the normal working baud rate
  delay(20);
 
  hc12ExitConfigMode();
  return response;
}
 
 
String hc12FormatTimestamp() {
  char buf[24];
  if (init_status.rtc_) {
    rtc_struct* rtc = get_rtc();
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
             rtc->year_, rtc->month_, rtc->day_, rtc->hour_, rtc->minute_, rtc->second_);
  } else {
    snprintf(buf, sizeof(buf), "uptime %lus", millis() / 1000);
  }
  return String(buf);
}
 
void hc12SaveLastResponse(const String &response) {
  prefs.begin("radio", false);
  prefs.putString("ts", hc12FormatTimestamp());
  prefs.putString("resp", response.length() > 0 ? response : "\u25B2 No response");
  prefs.end();
}
 
String hc12LoadLastResponseLine() {
  prefs.begin("radio", true); //read-only
  String timestamp = prefs.getString("ts", "");
  String response = prefs.getString("resp", "");
  prefs.end();
  if (timestamp.length() == 0) return "";
  return "Last response (" + timestamp + "): " + response;
}
 
void hc12LedOff() {
  setSignalLed(0, 0, 0, LED_OFF);
  updateSignalLed();
}
 
void hc12WarnAndCountdown() {
  setSignalLed(255, 255, 0, LED_BLINK, SIGNALLED_BRIGHTNESS, 300, 300);
  for (int i = HC12_DISCONNECT_COUNTDOWN_SEC; i >= 1; i--) {
    Serial.print(i);
    Serial.print("... ");
    unsigned long start = millis();
    while (millis() - start < 1000) {
      updateSignalLed(); 
      delay(20);
    }
  }
  Serial.println();
}
 
void hc12ShowResponseIndicator(const String &response) {
  if (response.indexOf("OK") != -1) {
    setSignalLed(0, 255, 0, LED_SOLID);
  } else if (response.length() > 0) {
    setSignalLed(255, 255, 255, LED_SOLID);
  } else {
    setSignalLed(255, 0, 0, LED_SOLID);
  }
  updateSignalLed();
  delay(2000);
}
 
void setRadio() {
  hc12LedOff(); 
 
  String lastLine = hc12LoadLastResponseLine();
  if (lastLine.length() > 0) {
    Serial.println(lastLine);
  }
  Serial.println("Type a command for HC-12:");
 
  while (!Serial.available()) {
    delay(50); 
  }
  String userInput = Serial.readStringUntil('\n');
  userInput.trim();
 
  if (userInput.length() == 0 || userInput.equalsIgnoreCase("SetRadio")) {
    evaluateSystemState();
    return;
  }
 
  Serial.print("Command entered: "); Serial.println(userInput);
  Serial.println();
  Serial.println("! DISCONNECT USB-to-TTL FROM THE SATELLITE NOW !");
  hc12WarnAndCountdown();
 
  // normal working baud rate
  hc12LedOff();
  String response = hc12SendATCommand(userInput, HC12_NORMAL_BAUD);
  hc12ShowResponseIndicator(response);
 
  // fallback baud rate 
  if (response.indexOf("OK") == -1) {
    hc12LedOff();
    response = hc12SendATCommand(userInput, HC12_FALLBACK_BAUD);
    hc12ShowResponseIndicator(response);
  }
 
  hc12SaveLastResponse(response);
 
  evaluateSystemState();
}