#pragma once

// Create AsyncWebServer object on port 80
static AsyncWebServer server(80);
// Create an Event Source on /events
static AsyncEventSource events("/events"); //https://randomnerdtutorials.com/esp32-web-server-sent-events-sse/

void onUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){}
void initServer()
{
  server.on("/switch", HTTP_GET, [](AsyncWebServerRequest *request) {
    isLedLight = !isLedLight;
    request->send_P(200, "text/plain", "Switch LED");
  });
  
 server.on("/capture", HTTP_GET, [](AsyncWebServerRequest *request) {
    isPhotoNeeded = true;
    request->send_P(200, "text/plain", "Taking Photo");
  });
  
  server.on("/renew", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("RENEW!!!");
    request->send(200, "OK","RENEW");
  }, onUpload);

  server.on("/saved-photo", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, FILE_PHOTO, "image/jpg", false);
  });
  

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
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
}

void sendEvents()
{
  events.send("ping",NULL,millis());
  events.send(String(WiFi.RSSI()).c_str(), "wifi_rssi",millis());
  events.send(String(nameProbe).c_str(),"name_probe",millis());
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
  events.send(String(numPosition).c_str(),"num_position",millis());
  
}
