//HC-12 radio module AT-configuration

#pragma once
 
const uint8_t NANO_CMD_RF_TURN = 2; // match `enum Commands_list` order on the Nano
const uint8_t NANO_CMD_RF_SET  = 3;
 
const uint32_t HC12_NORMAL_BAUD = 115200;
const uint32_t HC12_FALLBACK_BAUD = 9600;
const unsigned long HC12_RESPONSE_TIMEOUT = 1000;
const unsigned long HC12_MODE_SWITCH_DELAY = 50;  
 
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
 

void setRadio() {
  Serial.println("Type a command for HC-12: (type 'SetRadio' again to exit)");
 
  bool active = true;
  while (active) {
    if (!Serial.available()) {
      delay(50); 
      continue;
    }
 
    String userInput = Serial.readStringUntil('\n');
    userInput.trim();
    if (userInput.length() == 0) continue;
 
    if (userInput.equalsIgnoreCase("SetRadio")) {
      active = false;
      Serial.println("Exiting HC-12 configuration mode. Resuming telemetry.");
      break;
    }
 
    Serial.print("Command: "); Serial.println(userInput);
 
    // normal working baud rate
    String response = hc12SendATCommand(userInput, HC12_NORMAL_BAUD);
    if (response.indexOf("OK") != -1) {
      Serial.print("  Response: "); Serial.println(response);
      Serial.println("Type a command for HC-12: (type 'SetRadio' again to exit)");
      continue; //success, ready for the next AT-command
    }
    Serial.println(response.length() == 0 ? "  \u25B2 No response." : ("  \u25B2 Response: " + response));
 
    // fallback baud rate 
    Serial.print("  Retrying (9600): "); Serial.println(userInput);
    response = hc12SendATCommand(userInput, HC12_FALLBACK_BAUD);
    if (response.indexOf("OK") != -1) {
      Serial.print("  Response: "); Serial.println(response);
      Serial.println("Type a command for HC-12: (type 'SetRadio' again to exit)");
      continue;
    }
    Serial.println(response.length() == 0 ? "  \u25B2 No response." : ("  \u25B2 Response: " + response));
    Serial.println("  \u25B2 Configuration failed !");
    Serial.println("Type a command for HC-12: (type 'SetRadio' again to exit)");
  }
}
