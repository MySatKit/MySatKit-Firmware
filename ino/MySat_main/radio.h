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
const unsigned long HC12_MODE_SWITCH_DELAY = 50;   //after SET goes low 
const unsigned long HC12_EXIT_DELAY = 300;         //after SET goes high 
const unsigned long HC12_IDLE_GAP_MS = 300; //silence that marks the end of a reply (multi-line replies, e.g. AT+RX, have gaps between lines)

// 1 = remember the RAW bytes received in every attempt and print them after "Last response".
// Diagnostic only (writes to flash on every command), so it is OFF by default
#define HC12_DEBUG_RAW 0
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
  delay(HC12_EXIT_DELAY);
}

#if HC12_DEBUG_RAW
String hc12DebugLog = "";
String hc12DebugEscape(const String &raw) { 
  String out = "";
  for (size_t i = 0; i < raw.length(); i++) {
    char c = raw[i];
    if (c == '\r') out += "\\r";
    else if (c == '\n') out += "\\n";
    else if (c >= 32 && c < 127) out += c;
    else { char b[6]; snprintf(b, sizeof(b), "<%02X>", (uint8_t)c); out += b; }
  }
  return out;
}
#endif

String hc12WaitForResponse(unsigned long timeout_ms) {
  String response = "";
  unsigned long start = millis();
  unsigned long lastByteMs = start;
  while (millis() - start < timeout_ms) {
    while (Serial.available()) {
      response += (char)Serial.read();
      lastByteMs = millis();
    }
    if (response.indexOf("OK") != -1 && millis() - lastByteMs >= HC12_IDLE_GAP_MS) break;
  }
  return response;
}

// Sends one AT-command to HC-12 at the given baud rate and returns its raw reply.
String hc12SendATCommand(const String &atCmd, uint32_t baud) {
  hc12EnterConfigMode();

  Serial.flush();
  Serial.updateBaudRate(baud); 
  delay(20);
  while (Serial.available()) Serial.read(); 

  Serial.print(atCmd);
  String response = hc12WaitForResponse(HC12_RESPONSE_TIMEOUT);
#if HC12_DEBUG_RAW
  hc12DebugLog += "[" + String(baud) + "] " + hc12DebugEscape(response) + " | ";
#endif

  Serial.flush();
  Serial.updateBaudRate(HC12_NORMAL_BAUD); //always leave the UART back at the normal working baud rate
  delay(20);

  hc12ExitConfigMode();
  return response;
}

// Priority, best to worst: OK > OTHER (readable, no "OK") > INVALID (junk/garbage,
// no latin letters at all) > NONE (nothing arrived )
enum ResponseCategory { RESP_OK = 0, RESP_OTHER = 1, RESP_INVALID = 2, RESP_NONE = 3 };

struct ClassifiedResponse {
  ResponseCategory category;
  String text; 
};

bool hc12ContainsLatinLetter(const String &s) {
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) return true;
  }
  return false;
}

ClassifiedResponse classifyResponse(const String &raw) {
  if (raw.length() == 0) {
    return { RESP_NONE, "" }; //nothing arrived at all before the timeout
  }

  String trimmed = raw;
  trimmed.trim();

  if (trimmed.length() == 0) {
    return { RESP_INVALID, "" }; //something arrived but nothing visible/readable
  }

  int okPos = trimmed.indexOf("OK");
  if (okPos != -1) {
    String okText = trimmed.substring(okPos);
    okText.trim();
    return { RESP_OK, okText };
  }

  if (hc12ContainsLatinLetter(trimmed)) {
    return { RESP_OTHER, trimmed };
  }

  return { RESP_INVALID, "" }; 
}

ClassifiedResponse hc12PickBetter(const ClassifiedResponse &a, const ClassifiedResponse &b) {
  return (b.category < a.category) ? b : a;
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

// Turns a classified result into the text that gets saved/printed as "Last response"
String hc12DisplayText(const ClassifiedResponse &result) {
  switch (result.category) {
    case RESP_OK:
    case RESP_OTHER:
      return result.text;
    case RESP_INVALID:
      return "▲ Invalid response";
    case RESP_NONE:
    default:
      return "▲ No response";
  }
}

void hc12SaveLastResponse(const ClassifiedResponse &result) {
  prefs.begin("radio", false);
  prefs.putString("ts", hc12FormatTimestamp());
  prefs.putString("resp", hc12DisplayText(result));
  prefs.end();
}

#if HC12_DEBUG_RAW
void hc12SaveDebug() {
  prefs.begin("radio", false);
  prefs.putString("dbg", hc12DebugLog);
  prefs.end();
}
String hc12LoadDebugLine() {
  prefs.begin("radio", true);
  String d = prefs.getString("dbg", "");
  prefs.end();
  return d.length() ? "Raw attempts: " + d : "";
}
#endif

// Returns a ready-to-print line like "Last response (2026-09-12 22:15:03): OK+C050",
// or "" if nothing was ever saved yet.
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

// Blinks yellow for HC12_DISCONNECT_COUNTDOWN_SEC seconds while printing "5... 4... ..."
void hc12WarnAndCountdown() {
  setSignalLed(255, 255, 0, LED_BLINK, SIGNALLED_BRIGHTNESS, 300, 300);
  for (int i = HC12_DISCONNECT_COUNTDOWN_SEC; i >= 1; i--) {
    Serial.print(i);
    Serial.print("... ");
    unsigned long start = millis();
    while (millis() - start < 1000) {
      updateSignalLed(); //keep the blink animating while we block here
      delay(20);
    }
  }
  Serial.println();
}

// Shows the FINAL, single outcome for 2 seconds: green = OK, white = some other
// readable (non-OK) reply, red = invalid/garbage or no reply at all.
void hc12ShowResponseIndicator(const ClassifiedResponse &result) {
  switch (result.category) {
    case RESP_OK:
      setSignalLed(0, 255, 0, LED_SOLID);
      break;
    case RESP_OTHER:
      setSignalLed(255, 255, 255, LED_SOLID);
      break;
    case RESP_INVALID:
    case RESP_NONE:
    default:
      setSignalLed(255, 0, 0, LED_SOLID);
      break;
  }
  updateSignalLed();
  delay(2000);
}

// Runs the command at the normal baud rate first. The fallback baud rate is tried only when
// that gave no usable reply (silence or garbage)
ClassifiedResponse hc12RunAttempts(const String &cmd) {
  ClassifiedResponse result = classifyResponse(hc12SendATCommand(cmd, HC12_NORMAL_BAUD));

  if (result.category == RESP_NONE || result.category == RESP_INVALID) {
    hc12LedOff();
    result = hc12PickBetter(result, classifyResponse(hc12SendATCommand(cmd, HC12_FALLBACK_BAUD)));
  }
  return result;
}

// Called from console.h when the user types "SetRadio". One AT-command attempt per
// call. Call "SetRadio" again afterwards to send another command (or just to check
// the result of the previous one, or to cancel without sending anything)
void setRadio() {
  hc12LedOff(); 

  String lastLine = hc12LoadLastResponseLine();
  if (lastLine.length() > 0) {
    Serial.println(lastLine);
  }
#if HC12_DEBUG_RAW
  String dbgLine = hc12LoadDebugLine();
  if (dbgLine.length() > 0) Serial.println(dbgLine);
#endif
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
#if HC12_DEBUG_RAW
  hc12DebugLog = "";
#endif
  Serial.println("! DISCONNECT USB-to-TTL FROM THE SATELLITE NOW !");
  hc12WarnAndCountdown();

  hc12LedOff();
  ClassifiedResponse result = hc12RunAttempts(userInput);

  hc12ShowResponseIndicator(result);
  hc12SaveLastResponse(result);
#if HC12_DEBUG_RAW
  hc12SaveDebug();
#endif

  evaluateSystemState();
}