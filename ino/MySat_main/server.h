//ensures the operation of the MySat main console (web-interface)
#pragma once

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <LittleFS.h>  
#include "sensors_data.h"
#include "control.h"
#include "base64.h"

const char* htmlContent = R"###(
<!DOCTYPE html>
<html>
<head>
    <link href="/bootstrap.css" rel="stylesheet">
    <style>
        body {
      background-color: #008080;
      color: white;
	  overflow-x: hidden;
        }
        button{
     background-color:#0a0a23;
       color: white;
       border-radius:10px;
           width: 100px;
           height: 30px;
      }
      img {
        max-width: 100%;
      }
      .text-border{
       margin-bottom: 40px; 
       }
     * {
    margin: 0;
    padding: 0;
}


.stars, .twinkling {
  position:absolute;
  top:0;
  left:0;
  right:0;
  bottom:0;
  width:100%;
  height:100%;
  display:block;
}

.stars {
  background:#000 url(/apps_bg.png) repeat top center;
  z-index: 0;
}

.twinkling {
  background-image: url(/twinkling.png);;
  animation: 3s twinkle infinite;
  animation-timing-function: linear;
}

@keyframes twinkle {
  from {background-position: 0}
  to {background-position: -100px 0}
}


.text-data{
  border: 2px solid white;
  border-radius:20px;
  background:#ffffff42;
  text-align: center;
}
.text-height{
  height: 100px;
}

.text-data-sunLight{
  text-align: center;
}
.datetime_font{
  font-size: 20px;
}
.gas-res-border{
  border: 15px solid red;
  border-radius:50%;
  width:100%;
  position:absolute;
  top: 0;
  left: 0;
  bottom: 0;
  right: 0;

}
.gas-res-text{
  padding-top:35%;
  padding-bottom:35%;
  margin:0;
}

.box {	
  position: relative;
  width: 85%; /* ширина элемента */
  padding-top: 85%;
  margin: 0 auto 0 auto;
  top: 7%;
}

.content {
  position: absolute;
  top: 0;
  left: 0;
  bottom: 0;
  right: 0;
}





		.satelite-box {
			position: absolute;
			top: 10%;
			left: 0;
			bottom: 0;
			right: 0;
			margin:auto;
		}
		
		.satelite {
			transform-style: preserve-3d;
			transform: rotateY(199.8deg) rotateX(187.2deg) rotateZ(178.2deg);
			animation: cubeRotation 30s linear infinite;
			width: 100px;
			height: 100px;
			position: relative;
			margin:auto;
		}
		
		@keyframes cubeRotation {
			0% {transform: rotateY(0deg) rotateX(187.2deg) rotateZ(178.2deg);}
			100% {transform: rotateY(359.9deg) rotateX(187.2deg) rotateZ(178.2deg);}
		}
	
		.side  {
			width: 100px;
			height: 100px;
			position: absolute;
		}
		
		.front {
			background: blue;
			transform: translateZ(50px);
			background-image:url(/cube_front.png);
		}
		
		.left {
			background: yellow;
			transform: rotateY(-90deg) translateZ(50px);
			background-image:url(/cube_side.png);
		}
		
		.right {
			background: orange;
			transform: rotateY(90deg) translateZ(50px);
			background-image:url(/cube_side.png);
		}
		
		.top {
			background: green;
			transform: rotateX(90deg) translateZ(50px);
			background-image:url(/cube_plate.png);
		}
		
		.bottom {
			background: black;
			transform: rotateX(-90deg) translateZ(50px);
			background-image:url(/cube_plate.png);
		}
		
		.back {
			background: brown;
			transform: rotateY(180deg) translateZ(50px);
			background-image:url(/cube_shield.png);
		}
		
		.rightwing {
			background: LightGreen;
			transform-origin: 50% 0%;
			transform: rotateY(90deg) translateZ(60px) rotateX(3deg);
			background-image:url(/cube_solar.png);
			width: 80px;
		}
		.leftwing {
			background: LightGreen;
			transform-origin: 50% 0%;
			transform: rotateY(-90deg) translateZ(40px) rotateX(3deg);
			background-image:url(/cube_solar.png);
			width: 80px;
		}
		.leftwingback { /*днище левого крыла*/
			background: gray;
			transform-origin: 50% 0%;
			transform: rotateY(-90deg) translateZ(40px) rotateX(3deg);
			width: 80px;
		}
		.rightwingback { /*днище правого крыла*/
			background: gray;
			transform-origin: 50% 0%;
			transform: rotateY(90deg) translateZ(50px) rotateX(3deg);
			width: 80px;
		}


    </style>

</head>
<body>
  <div class="stars"></div>
  <div class="twinkling">
	<table>
		<tr>
			<td>
				<h1>MySat</h1>
			</td>
			<td>
				<h6 style="padding-left: 20px; vertical-align:bottom;"></h6>
			</td>
		</tr>
	</table>
    <div class="container-fluid">
    <div class="row">
      <div class="col-lg-5">
        <img src="/photo_placeholder.png" alt="" id = "photoFromESP">
  <span id = "date_time_mysat" class = "datetime_font">Date and time: </span>
      </div>
      <div class = "col-lg-7">
        <div class="container-fluid">
          <div class = "row">
            <div class="col-lg-4 text-data  m-2">
              <h3 id = "text_data">Temperature:</h3>
               <h4 id = "temperature">0.0</h4>
            </div>  
            <div class="col-lg-4 text-data  m-2">
              <h3 id = "text_data">Pressure:</h3>
              <h3 id = "pressure">0</h3>
            </div>
            <div class="col-lg-3 text-data  m-2">
              <h3 id = "text_data">Humidity:</h3>
              <h3 id = "humidity">0</h3>
            </div>
             <!-- <div class="col-lg-2">
              <h3 id = "text_data">Gas resistance:</h3>
              <h3 id = "gas_resistance">0</h3>
            </div>  -->
          </div>
          <div class = "row">
            <div class = "col-lg-4 text-data m-2">
              <h3 id = "text_data">X:</h3>
              <h4 id = "roll">0</h3>
            </div>
            <div class = "col-lg-4 text-data m-2">
              <h3 id = "text_data">Y:</h3>
              <h4 id = "pitch">0</h3>
            </div>
            <div class = "col-lg-3 text-data m-2">
              <h3 id = "text_data">Z:</h3>
              <h4 id = "yaw">0</h3>
            </div>
            
          </div>
          <div class = "row">
            <div class = "col-lg-4 text-data m-2">
              <div class="text-data-sunLight">
  <h3>Sunlight trackers</h3>


  <svg id="sunSvg" viewBox="-120 -120 240 240" style="width:80%;margin-left:10%;display:block;">
    <defs>
 
      <radialGradient id="g1"><stop offset="0%" stop-color="white"/><stop id="g1s" offset="100%" stop-color="gray"/></radialGradient>
      <radialGradient id="g2"><stop offset="0%" stop-color="white"/><stop id="g2s" offset="100%" stop-color="gray"/></radialGradient>
      <radialGradient id="g3"><stop offset="0%" stop-color="white"/><stop id="g3s" offset="100%" stop-color="gray"/></radialGradient>
      <radialGradient id="g4"><stop offset="0%" stop-color="white"/><stop id="g4s" offset="100%" stop-color="gray"/></radialGradient>
   
      <radialGradient id="sunGrad"><stop offset="0%" stop-color="yellow"/><stop offset="100%" stop-color="gold"/></radialGradient>
    </defs>
 
    <circle r="110" fill="none" stroke="rgba(255,255,255,0.2)" />
    <circle r="80"  fill="none" stroke="rgba(255,255,255,0.15)" />
    <circle r="50"  fill="none" stroke="rgba(255,255,255,0.1)" />

    <path id="sec1" fill="url(#g1)"></path>
    <path id="sec2" fill="url(#g2)"></path>
    <path id="sec3" fill="url(#g3)"></path>
    <path id="sec4" fill="url(#g4)"></path>

    <circle r="112" fill="none" stroke="white" stroke-width="2" />

   
    <circle id="sunDot" r="9" fill="url(#sunGrad)" visibility="hidden"></circle>
  </svg>
</div>
            </div>
      <div class = "col-lg-4 text-data m-2">
	  <h3>Status</h3>
		<div class="box">
			<div class="satelite-box">
				<div class="satelite">
					<div class="side front">
					</div>
					<div class="side left">
					</div>
					<div class="side right">
					</div>
					<div class="side top">
					</div>
					<div class="side bottom">
					</div>
					<div class="side back">
					</div>
					<div class="side leftwing">
					</div>
					<div class="side leftwingback">
					</div>
					<div class="side rightwing">
					</div>
					<div class="side rightwingback">
					</div>
				</div>
			</div>
		</div>
      </div>
	  <div class = "col-lg-3 text-data m-2">
			<h3>Air quality</h3>
            <div class="box">
				<div class = "gas-res-border" id = "gas-res-border">
					<h3 class = "gas-res-text" id ="gas-res-text">47.5%</h3>
                </div>
			</div>
        </div>
      </div>
          </div>

        </div>
      </div>
    </div>
    <div class="row">
    <div class="col-lg-4">
            Turn:
            <button onclick='toggleLED()'>Turn LED</button>
            <button onclick='toggleMotor()'>Turn Motor</button>
            <button onclick='getPhoto()'>Get Photo</button>
    </div>
        
    </div>    
    </div>
    </div>
    <script src="/bootstrap.js"></script>
    
    <script>
	
		var wing_angle = 3; //крылья изначально открыты на 3 градуса
		var motor_direction = 1;

        function arrayBufferToBase64(buff) {
          var binary = '';
          var bytes = new Uint8Array(buff);
          var len = bytes.byteLength;

          for (var i = 0; i < len; i++) {
            binary += String.fromCharCode(bytes[i]);
          }

          return btoa(binary);
        }
        function toggleLED() {
            var xhttp = new XMLHttpRequest();
            xhttp.open('GET', '/light_on', true);
            xhttp.send();
        }
        function getPhoto() {
          var xhttp = new XMLHttpRequest();
          xhttp.onreadystatechange = function() {
          if (xhttp.readyState === 4) {
            if (xhttp.status === 200) {
                var base64Data = xhttp.responseText;
                var img = document.getElementById('photoFromESP');
                img.src = 'data:image/jpeg;base64,' + base64Data;
                
                var photoTimestamp = xhttp.getResponseHeader('X-Photo-Timestamp');
                
                if (photoTimestamp) {
                    var photoDate = new Date(photoTimestamp);
                    var photoTimeString = photoDate.toLocaleString();
                    var photoTimeElement = document.getElementById('photo_timestamp');
                    if (!photoTimeElement) {
                        photoTimeElement = document.createElement('div');
                        photoTimeElement.id = 'photo_timestamp';
                        photoTimeElement.className = 'datetime_font';
                        photoTimeElement.style.marginTop = '10px';
                        photoTimeElement.style.color = '#ffffff';
                        var photoContainer = img.parentNode;
                        photoContainer.appendChild(photoTimeElement);
                    }
                    
                    photoTimeElement.textContent = 'Photo taken: ' + photoTimeString;
                    
                    console.log('Photo timestamp:', photoTimestamp);
                } else {
                    console.warn('No photo timestamp received');
                }
                
            } else {
                console.error('Error fetching photo');
            }
          }
        };
    
      xhttp.open('GET', '/get_photo', true);
      xhttp.send();
      }
        function toggleMotor() { //TURN WING
            var xhttp = new XMLHttpRequest();
            xhttp.open('GET', '/motor_on', true);
            xhttp.send();
        }
		
	function turning() { 
		
		if (( (motor_direction==1) && (wing_angle<75)  || ((motor_direction==-1) && (wing_angle > 3)) )) { 
			wing_angle+=motor_direction;
			document.querySelector('.rightwing').style.transform = 'rotateY(90deg) translateZ(60px) rotateX(' + wing_angle + 'deg)';
			document.querySelector('.leftwing').style.transform = 'rotateY(-90deg) translateZ(40px) rotateX(' + wing_angle + 'deg)';
			document.querySelector('.rightwingback').style.transform = 'rotateY(90deg) translateZ(60px) translateY(1px) rotateX(' + wing_angle + 'deg)';
			document.querySelector('.leftwingback').style.transform = 'rotateY(-90deg) translateZ(40px) translateY(1px) rotateX(' + wing_angle + 'deg)';
			
			setTimeout(turning, 20);
		}
	}		

 const R_MIN = 0;    
 const R_MAX = 100;    
 const SUN_DOT_R = 112; 
 const HIDE_THRESHOLD = 0.10; 

 function brightnessFromRaw(raw) {
  return (1023 - Math.max(0, Math.min(1023, raw))) / 1023;
 }

 function polarToCartesian(r, angleDeg) {
  const a = (angleDeg - 90) * Math.PI / 180;
  return { x: r * Math.cos(a), y: r * Math.sin(a) };
 }

 function sectorPath(r, startAngle, endAngle) {
  const start = polarToCartesian(r, startAngle % 360);
  const end = polarToCartesian(r, endAngle % 360);
  let sweepFlag = 1;
  let largeArc = ((endAngle - startAngle + 360) % 360) > 180 ? 1 : 0;
  return `M 0 0 L ${start.x} ${start.y} A ${r} ${r} 0 ${largeArc} ${sweepFlag} ${end.x} ${end.y} Z`;
 }

 function updateLightChart(ph1, ph2, ph3, ph4) {
  const b = [
    brightnessFromRaw(ph1), 
    brightnessFromRaw(ph2), 
    brightnessFromRaw(ph3), 
    brightnessFromRaw(ph4)  
  ];

  const sectors = [
    { id: "sec1", gStop: "g1s", a0: 225, a1: 315 },
    { id: "sec2", gStop: "g2s", a0: 135, a1: 225 },
    { id: "sec3", gStop: "g3s", a0: 45,  a1: 135 },
    { id: "sec4", gStop: "g4s", a0: 315, a1: 45 } 
  ];

  sectors.forEach((s, i) => {
    const r = R_MIN + b[i] * (R_MAX - R_MIN);
    document.getElementById(s.id).setAttribute("d", sectorPath(r, s.a0, s.a1));

    const c = Math.round(100 + 155 * b[i]);
    document.getElementById(s.gStop).setAttribute("stop-color", `rgb(${c},${c},${c})`);
  });

  const angles = [270, 180, 90, 0]; 
  let vx = 0, vy = 0;
  b.forEach((val, i) => {
    const rad = angles[i] * Math.PI / 180;
    vx += val * Math.cos(rad);
    vy += val * Math.sin(rad);
  });

  const mag = Math.sqrt(vx*vx + vy*vy);
  const normalized = mag / 4;
  const sun = document.getElementById("sunDot");

  if (normalized > HIDE_THRESHOLD) {
    const angle = Math.atan2(vy, vx) * 180 / Math.PI;
    const pos = polarToCartesian(SUN_DOT_R, angle);
    sun.setAttribute("cx", pos.x);
    sun.setAttribute("cy", pos.y);
    sun.setAttribute("visibility", "visible");
  } else {
    sun.setAttribute("visibility", "hidden");
  }
 }
		
		
  function fetchDataPeriodically() {
      setInterval(function() {
        var xhttp = new XMLHttpRequest();

        xhttp.onreadystatechange = function() {
            if (xhttp.readyState === 4) {
                if (xhttp.status === 200) {
                    var responseData = JSON.parse(xhttp.responseText);
                    let target = responseData.motor_state ? 75 : 3;

                    if (wing_angle !== target) {
                      motor_direction = wing_angle < target ? 1 : -1;
                      turning();
                    }
                    console.log('Received JSON data:', responseData);
                    document.getElementById("temperature").textContent = responseData.temperature.toFixed(2) + ' C';
                    document.getElementById("pressure").textContent = responseData.pressure.toFixed(2)+ ' Pa';
                    document.getElementById("humidity").textContent = responseData.humidity.toFixed(2) + ' %';
                    
                    document.getElementById("roll").textContent = Math.round(responseData.roll);
                    document.getElementById("pitch").textContent = Math.round(responseData.pitch);
                    document.getElementById("yaw").textContent = Math.round(responseData.yaw);

        let gas_res = responseData.gas_resistance;
        let gas_res_percent = (gas_res/6).toFixed(1);
        document.getElementById("gas-res-text").textContent = gas_res_percent + "%";
        
                    if(gas_res > 250){
                  document.getElementById("gas-res-border").style.border = "15px solid #00b910";
        }

        if(gas_res >= 150 && gas_res <= 250){
                  document.getElementById("gas-res-border").style.border = "15px solid #9dff55";
        }

        if(gas_res >= 80 && gas_res <= 150){
                  document.getElementById("gas-res-border").style.border = "15px solid #edff00";
        }
      
        if(gas_res >= 30 && gas_res <= 80){
                  document.getElementById("gas-res-border").style.border = "15px solid #ffb300";
        }

        if(gas_res >= 20 && gas_res <= 30){
                  document.getElementById("gas-res-border").style.border = "15px solid #ff0000";
        }

        if(gas_res < 20){
                  document.getElementById("gas-res-border").style.border = "15px solid #000000";
        }
      
                  updateLightChart(
                   responseData.ph1,
                   responseData.ph2,
                   responseData.ph3,
                   responseData.ph4
                  );
                
    let year = responseData.year_;
    let month = responseData.month_;
    let day = responseData.day_; 
    let hour = responseData.hour_;    
    let minute = responseData.minute_;
    let second = responseData.second_;      
    date_time_string = new Date(year, month - 1, day, hour, minute, second);
    document.getElementById("date_time_mysat").textContent = 'Date and time: ' + date_time_string.toLocaleString();
    console.log(date_time_string);

                } else {
                    console.error('Error fetching data');
                }
            }
        };
        xhttp.open('GET', '/get_data', true);
        xhttp.send();
    }, 2000);

       
}
    

fetchDataPeriodically();

    </script>
</body>
</html>
)###";


WebServer server(80);

String json_string = "";

const size_t bufferSize = JSON_OBJECT_SIZE(35);

DynamicJsonDocument json_sensors(bufferSize);

String* generateSensorsDataJson(pointer_of_sensors* data_, bool motor_state) {
  json_string = "";
  if (init_status.ads_) {
    json_sensors["ph1"] = data_->ads_->ph1;
    json_sensors["ph2"] = data_->ads_->ph2;
    json_sensors["ph3"] = data_->ads_->ph3;
    json_sensors["ph4"] = data_->ads_->ph4;
  } else {
    json_sensors["ph1"] = 0;
    json_sensors["ph2"] = 0;
    json_sensors["ph3"] = 0;
    json_sensors["ph4"] = 0;
  }
  if (init_status.bme_) {
    json_sensors["temperature"] = data_->bme_->temperature;
    json_sensors["humidity"] = data_->bme_->humidity;
    json_sensors["gas_resistance"] = data_->bme_->gas_resistance;
    json_sensors["pressure"] = data_->bme_->pressure;
  } else {
    json_sensors["temperature"] = 0;
    json_sensors["humidity"] = 0;
    json_sensors["gas_resistance"] = 0;
    json_sensors["pressure"] = 0;
  }
  if (init_status.rtc_) {
    json_sensors["year_"] = data_->rtc_->year_;
    json_sensors["month_"] = data_->rtc_->month_;
    json_sensors["day_"] = data_->rtc_->day_;
    json_sensors["hour_"] = data_->rtc_->hour_;
    json_sensors["minute_"] = data_->rtc_->minute_;
    json_sensors["second_"] = data_->rtc_->second_;
  } else {
    json_sensors["year_"] = 0;
    json_sensors["month_"] = 0;
    json_sensors["day_"] = 0;
    json_sensors["hour_"] = 0;
    json_sensors["minute_"] = 0;
    json_sensors["second_"] = 0;
  }
  if (init_status.mpu_) {
    json_sensors["yaw"] = data_->mpu_->yaw;
    json_sensors["pitch"] = data_->mpu_->pitch;
    json_sensors["roll"] = data_->mpu_->roll;
  } else {
    json_sensors["yaw"] = 0;
    json_sensors["pitch"] = 0;  
    json_sensors["roll"] = 0;
  }
  json_sensors["motor_state"] = motor_state;
  serializeJson(json_sensors, json_string);
  //Serial.println(json_string);
  return &json_string;
}

void handleRoot() {
  Serial.println(json_string);
  server.send(200, "text/html", htmlContent);
}

void handleGetData() {
  pointer_of_sensors* data = get_sensors_data();
  generateSensorsDataJson(data, stateMotor);
  server.send(200, "text/plain", json_string);
}

void handleGetPhoto() {
  Serial.println("Create Photo");
  rtc_struct* current_time = get_rtc();
  camera_fb_t* fb = esp_camera_fb_get();

  while (!fb) {
    fb = esp_camera_fb_get();
    if (fb) {
      break;
    }
  }
  Serial.print("Get photo length: ");
  Serial.println(fb->len);
  String base64String = base64::encode(fb->buf, fb->len);
  //Serial.println(base64String);
  char timestamp[25];
  snprintf(timestamp, sizeof(timestamp), "%04d-%02d-%02dT%02d:%02d:%02d",
           current_time->year_,
           current_time->month_,
           current_time->day_,
           current_time->hour_,
           current_time->minute_,
           current_time->second_);
  server.sendHeader("X-Photo-Timestamp", timestamp);
  server.sendHeader("Cache-Control", "no-cache");

  server.send(200, "text/plain", base64String);

  Serial.print("Photo sent with timestamp: ");
  Serial.println(timestamp);

  esp_camera_fb_return(fb);
}

bool stateLight = false;

void light_on() {
  stateLight = !stateLight;
  control_light(stateLight);
}

void motor_on() {
  setStateMotor(!stateMotor);
}

void initServer() {
  server.on("/", handleRoot);
  server.on("/get_data", handleGetData);
  server.on("/light_on", HTTP_GET, light_on);
  server.on("/motor_on", HTTP_GET, motor_on);
  server.on("/get_photo", HTTP_GET, handleGetPhoto);

  server.onNotFound([]() {
    String path = server.uri();
    if (LittleFS.exists(path)) {
      String contentType = "text/plain";
      if (path.endsWith(".html")) contentType = "text/html";
      else if (path.endsWith(".css")) contentType = "text/css";
      else if (path.endsWith(".js")) contentType = "application/javascript";
      else if (path.endsWith(".png")) contentType = "image/png";
      else if (path.endsWith(".jpg") || path.endsWith(".jpeg")) contentType = "image/jpeg";
      else if (path.endsWith(".gif")) contentType = "image/gif";
      else if (path.endsWith(".svg")) contentType = "image/svg+xml";

      File file = LittleFS.open(path, "r");
      server.streamFile(file, contentType);
      file.close();
      return;
    }
    server.send(404, "text/plain", "File Not Found");
  });

  server.begin();
  Serial.println("HTTP server started");
}
