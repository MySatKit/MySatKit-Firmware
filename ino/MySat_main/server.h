//ensures the operation of the MySat main console (web-interface)
#pragma once

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <LittleFS.h>
#include "sensors_data.h"
#include "control.h"
#include "base64.h"

extern String callSign;

const char* htmlContent = R"###(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <link href="/bootstrap.css" rel="stylesheet">
    <style>
        body {
      background-color: #008080;
      color: white;
      overflow-x: hidden;
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
  padding-top:28%;
  padding-bottom:0;
  margin:0;
  font-size: 24px;
}

#gasResistance {
  position: absolute; 
  top: 55%;
  left: 50%; 
  transform: translateX(-50%);
  font-size: 10px;
  color: #CCCCCC; 
  margin: 0;
  padding: 0;
  width: 100%;
}

.box { 
  position: relative;
  width: 85%; 
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
    .leftwingback {
      background: gray;
      transform-origin: 50% 0%;
      transform: rotateY(-90deg) translateZ(40px) rotateX(3deg);
      width: 80px;
    }
    .rightwingback { 
      background: gray;
      transform-origin: 50% 0%;
      transform: rotateY(90deg) translateZ(50px) rotateX(3deg);
      width: 80px;
    }

.control-btn {
  background-color: #0a0a23;     
  color: white;
  border-radius: 10px;
  height: 34px;
  font-size: 14px;
  border: none;
  padding: 0 12px;
  line-height: 34px;            
  cursor: pointer;
}
#photoButtons {
  display: flex;
  justify-content: center;
  gap: 6px;
  margin-top: 6px;
}

#photoButtons button {
  width: 44px;
  padding: 0;
}
#photoButtons button.active {
  background-color: #d6d6d6;
  color: black;
  box-shadow: inset 0 0 0 2px rgba(255,255,255,0.7);
}
.pressable:active {
  filter: brightness(1.25);
  transform: translateY(1px);
}
#logBtn {
  background-color: #4a4a4a;
}
#logMenu {
  display: none;
  position: absolute;
  bottom: 45px;
  right: 0;
  background: #0a0a23;
  border: 2px solid white;
  border-radius: 10px;
  padding: 10px;
  z-index: 1000;
  min-width: 250px;
}
#logMenu p {
  font-size: 12px;
  margin-bottom: 5px;
}
#logFilesList {
  width: 100%;
  margin-bottom: 10px;
  font-size: 12px;
  color: white;
  background: #222;
  border: 1px solid white;
}
.log-download-btn {
  width: 100%;
  background-color: #d6d6d6;
  color: #000;
}
#connectionStatus {
  font-size: 14px;
  display: flex;
  align-items: center;
  gap: 8px;
}

#statusDot {
  width: 10px;
  height: 10px;
  border-radius: 50%;
  display: inline-block;
  transition: all 0.3s ease;
}

#statusText {
  font-weight: 500;
  transition: color 0.3s ease;
}

@keyframes pulse {
  0%, 100% { opacity: 1; }
  50% { opacity: 0.4; }
}

.status-active {
  background-color: #00FF00;
  box-shadow: 0 0 8px rgba(0, 255, 0, 0.8);
}

.text-active {
  color: #00FF00;
}

.status-waiting {
  background-color: #FFA500;
  box-shadow: 0 0 8px rgba(255, 165, 0, 0.8);
  animation: pulse 2s infinite;
}

.text-waiting {
  color: #FFA500;
}

.status-lost {
  background-color: #FF0000;
  box-shadow: 0 0 8px rgba(255, 0, 0, 0.8);
  animation: pulse 1s infinite;
}

.text-lost {
  color: #FF0000;
}

.charging-label {
  font-size: 11px;          
  color: #CCCCCC;          
  font-weight: 100;        
  text-transform: uppercase; 
  margin-top: -8px;        
  min-height: 12px;         
  display: block;
}

.solar-bar-style {
  font-size: 13px;        
  letter-spacing: 2px;      
  color: #FFFFFF;         
  margin-top: -7px;      
  line-height: 1;
}

    </style>

</head>
<body>
  <div class="stars"></div>
  <div class="twinkling">
    <table>
      <tr>
        <td>
          <div style="display: flex; align-items: center; gap: 20px;">
            <h1 id="callSign">MySat</h1>
  
            <div id="connectionStatus">
              <span id="statusDot"></span>
              <span id="statusText">Communication session active</span>
            </div>
          </div>
        </td>
        <td>
          <h6 style="padding-left: 20px; vertical-align:bottom;"></h6>
        </td>
      </tr>
    </table>
    <div class="container-fluid">
    <div class="row">
      <div class="col-lg-5">
        <img src="/photo_placeholder.png" alt="" id="photoFromESP">
        <div id="photoInfo" class="datetime_font" style="margin-top:10px;">
          Photo №:, taken:
        </div>
        <div id="photoButtons" style="margin-top:10px;"></div>
    
      <span id="date_time_mysat" class="datetime_font" style="display: block; margin-top: 10px;">
        Date and time:
    </span>
    </div>
      <div class = "col-lg-7">
        <div class="container-fluid">
          <div class = "row justify-content-start">
            <div class="col-lg-4 text-data  m-2">
              <h3 id = "text_data">Temperature:</h3>
                <h4 id = "temperature">0.0</h4>
            </div>  
            <div class="col-lg-4 text-data  m-2">
              <h3 id = "text_data">Battery:</h3>
              <h4 id = "battery_v">0.00 V</h4>
              <div id = "charging_status" class="charging-label"></div>
            </div>
            <div class="col-lg-3 text-data  m-2">
              <h3 id = "text_data" style="white-space: nowrap;">Solar panels:</h3>
              <h4 id = "solarPanels_v">0.00 V</h4>
              <div id="solar_bar" class="solar-bar-style"></div>
            </div>
          </div>
          <div class = "row">
            <div class = "col-lg-4 text-data m-2">
              <h3 id = "text_data">X:</h3>
              <h4 id = "roll">0</h4>
            </div>
            <div class = "col-lg-4 text-data m-2">
              <h3 id = "text_data">Y:</h3>
              <h4 id = "pitch">0</h4>
            </div>
            <div class = "col-lg-3 text-data m-2">
              <h3 id = "text_data">Z:</h3>
              <h4 id = "yaw">0</h4>
            </div>
            
          </div>
          <div class = "row">
            <div class = "col-lg-4 text-data m-2">
              <div class="text-data-sunLight">
    <h3>Sun Tracker</h3>

    <svg id="sunSvg" viewBox="-160 -160 320 320" style="width:100%; display:block;">
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

      <circle r="112" fill="none" stroke="white" stroke-width="2.5" />

      <style>
        .label-main { font-family: sans-serif; font-weight: bold; fill: white; font-size: 15px; }
        .label-sub { font-family: sans-serif; font-weight: normal; fill: #ccc; font-size: 12px; }
      </style>

      <g transform="translate(0, -125)">
        <g transform="translate(-11, -30) scale(0.9)" fill="none" stroke="white" stroke-width="1.5">
            <rect x="0" y="0" width="22" height="15" rx="2" />
            <circle cx="11" cy="7.5" r="4.5" />
            <path d="M7,0 L8,-3 L14,-3 L15,0" />
            <circle cx="18" cy="4" r="1" fill="white" stroke="none" />
        </g>
        <text x="0" y="5" text-anchor="middle">
            <tspan class="label-main">PH4</tspan>
            <tspan class="label-sub"> · +X</tspan>
        </text>
      </g>

      <g transform="translate(0, 135)">
        <text x="0" y="5" text-anchor="middle">
            <tspan class="label-main">PH2</tspan>
            <tspan class="label-sub"> · −X</tspan>
        </text>
        <g transform="translate(40, -30) scale(0.06)" fill="none" stroke="white" stroke-width="25">
            <path d="M493.52,581.67c-9-12.42-19.26-23.94-30.61-34.27c-4.24-3.86-31.98-20.19-31.3-25.1
            c0.03-0.22,0.06-0.45,0.09-0.67c4.74-33.69,5.26-66.97,4.62-101.09c-0.96-51.5-6.5-103.47-23.48-152.37
            c-6.36-18.3-14.27-36.05-23.14-53.27c-10.52-20.42-22.54-40.23-37.31-57.89c-13.81-16.51-31-27.21-46.46-41.51v-0.03
            c-0.01,0.01-0.01,0.01-0.02,0.02c-0.01-0.01-0.01-0.01-0.02-0.02v0.03c-15.46,14.3-32.64,25-46.44,41.51
            c-14.76,17.66-26.78,37.47-37.29,57.89c-8.87,17.23-16.78,34.97-23.13,53.27c-16.98,48.89-22.51,100.87-23.47,152.37
            c-0.64,34.12-0.12,67.4,4.62,101.09c0.03,0.22,0.06,0.45,0.09,0.67c0.69,4.91-27.04,21.24-31.28,25.1
            c-11.34,10.33-21.59,21.85-30.59,34.27c-28.8,39.76-44.01,88.5-43.39,137.56c0.27,21.7,1.69,50.79,12.11,70.29
            c0.32,0.61,8.73-12.75,9.22-13.53c3.52-5.57,7.29-10.98,11.27-16.23c8.23-10.85,17.4-20.98,27.37-30.26
            c24.14-22.48,52.28-38.37,82.71-50.54c-0.18,0.07,11.77,20.89,12.91,22.49c5.72,7.99,12.72,15.04,20.59,20.91
            c15.72,11.72,35.04,18.79,54.71,19.05v0c0.01,0,0.02,0,0.03,0c0.01,0,0.02,0,0.03,0v0c19.68-0.26,39.01-7.33,54.73-19.05
            c7.87-5.87,14.88-12.92,20.6-20.91c1.14-1.6,13.1-22.41,12.92-22.49c30.45,12.17,58.6,28.06,82.75,50.54
            c9.97,9.28,19.14,19.41,27.38,30.26c3.98,5.25,7.75,10.66,11.27,16.23c0.5,0.79,8.9,14.14,9.23,13.53
            c10.42-19.49,11.84-48.59,12.12-70.29C537.55,670.17,522.34,621.43,493.52,581.67z M305.95,380.09c-29.51,0-53.43-23.92-53.43-53.43
            s23.92-53.43,53.43-53.43s53.43,23.92,53.43,53.43S335.46,380.09,305.95,380.09z"/>
        </g>
      </g>

      <g transform="translate(-125, 0)">
        <text x="0" y="-5" text-anchor="end" class="label-main">PH1</text>
        <text x="0" y="12" text-anchor="end" class="label-sub">+Y</text>
      </g>

      <g transform="translate(125, 0)">
        <text x="0" y="-5" text-anchor="start" class="label-main">PH3</text>
        <text x="0" y="12" text-anchor="start" class="label-sub">−Y</text>
      </g>
      
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
          <h3 class = "gas-res-text" id ="gas-res-text">IAQ</h3>
          <p id="gasResistance" class="gas-res-subtext"></p>
                </div>
      </div>
      <p id="data-quality-text" style="font-size: 14px; margin-top: 25px;"></p>
        </div>
          <div class="col-lg-12" style="text-align:center; margin-top:10px;">
            <button class="control-btn pressable" onclick="toggleLED()">Turn LED</button>
            <button class="control-btn pressable" onclick="toggleMotor()">Turn Motor</button>
            <button id="photoBtn" class="control-btn pressable" onclick="getPhoto()">Get Photo</button>
            <div style="display: inline-block; margin-left: 10px; position: relative;">
              <button id="logBtn" class="control-btn pressable" onclick="toggleLogMenu()">
                Mission Logs ▾
              </button>
              <div id="logMenu">
                <p>Select log file:</p>
                <select id="logFilesList" class="control-btn">
                  <option>Loading logs...</option>
                </select>
                <button class="control-btn pressable log-download-btn" onclick="downloadSelectedLog()">
                  Download Selected (.csv)
                </button>
              </div>
            </div>
        </div>

      </div>
          </div>
        </div>
      </div>
    </div>
    </div>
    </div>
    <script src="/bootstrap.js"></script>
    
    <script>
    
    var wing_angle = 3; 
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
          console.log("TurnLED CLICK:", new Date().toISOString());

          var xhttp = new XMLHttpRequest();

          xhttp.addEventListener("loadstart", () => {
            console.log("TurnLED HTTP SENT:", new Date().toISOString());
          });

          xhttp.onload = () => {
            console.log("TurnLED RESPONSE RECEIVED:", new Date().toISOString());
          };

          xhttp.open('GET', '/light_on', true);
          xhttp.send();
        }

    let availablePhotoIds = [];
    let lastPhotoId = null;

    function highlightActivePhotoButton(id) {
      const buttons = document.querySelectorAll("#photoButtons button");
      buttons.forEach(btn => {
        if (btn.textContent == id) {
          btn.classList.add("active");
        } else {
          btn.classList.remove("active");
        }
      });
    }

    function updatePhotoButtons() {
      let container = document.getElementById("photoButtons");
      container.innerHTML = "";

      let total = availablePhotoIds.length;
      let displayIds = [];

      if (total <= 10) {
        displayIds = availablePhotoIds.slice().reverse();
      } else {
        displayIds = availablePhotoIds.slice(total - 10).reverse();
      }

      for (let i = 0; i < 10; i++) {
        let btn = document.createElement("button");
        btn.classList.add("control-btn", "pressable");

        if (displayIds[i] !== undefined) {
          btn.textContent = displayIds[i];
          btn.onclick = () => loadPhotoById(displayIds[i]);
        } else {
          btn.textContent = "-";
          btn.disabled = true;
        }

        container.appendChild(btn);
      }
    }

    function fetchPhotoList() {
      let xhttp = new XMLHttpRequest();
      xhttp.onload = function () {
        if (xhttp.status === 200) {
          let data = JSON.parse(xhttp.responseText);
          availablePhotoIds = data.ids || [];
          updatePhotoButtons();
        }
      };
      xhttp.open("GET", "/get_photo_list", true);
      xhttp.send();
    }

    function loadPhotoById(id) {
      let xhttp = new XMLHttpRequest();
      xhttp.onload = function () {
        if (xhttp.status === 200) {
          let data = JSON.parse(xhttp.responseText);

          document.getElementById("photoFromESP").src =
            "data:image/jpeg;base64," + data.image;
          
          let nice = "Time unknown(module RTC not found)";
          if(data.timestamp && data.timestamp !== "unknown"){
            const date = new Date(data.timestamp);
            nice = date.toLocaleString("uk-UA");
          }

          document.getElementById("photoInfo").textContent =
             `Photo №${data.id}, taken: ${nice}`;

          lastPhotoId = data.id;
          highlightActivePhotoButton(data.id);

        }
      };
      xhttp.open("GET", "/get_photo_by_id?id=" + id, true);
      xhttp.send();
    }

    function getPhoto() {
      const xhttp = new XMLHttpRequest();

      xhttp.onload = function () {
        if (xhttp.status !== 200) {
          console.error("Photo error");
          return;
        }

        const data = JSON.parse(xhttp.responseText);

        document.getElementById("photoFromESP").src =
          "data:image/jpeg;base64," + data.image;

        let nice = "Time unknown(module RTC not found)";
        if(data.timestamp && data.timestamp !== "unknown"){
          const date = new Date(data.timestamp);
          nice = date.toLocaleString("uk-UA");
        }

        document.getElementById("photoInfo").textContent =
          `Photo №${data.id}, taken: ${nice}`;

        availablePhotoIds.push(data.id);
        updatePhotoButtons();

        lastPhotoId = data.id;
        highlightActivePhotoButton(data.id);
      };

      xhttp.open("GET", "/get_photo", true);
      xhttp.send();
    }

    function toggleLogMenu() {
      const menu = document.getElementById('logMenu');
      if (menu.style.display === 'none') {
        menu.style.display = 'block';
        fetchLogList(); 
      } else {
        menu.style.display = 'none';
      }
    }

    function fetchLogList() {
      const list = document.getElementById('logFilesList');
      list.innerHTML = '<option>Searching files...</option>';

      fetch('/get_log_list')
        .then(response => response.json())
        .then(data => {
            if (!data.files || data.files.length === 0) {
                list.innerHTML = '<option>No logs found</option>';
                return;
            }
            
            list.innerHTML = '';
            data.files.sort((a, b) => b.name.localeCompare(a.name));

            data.files.forEach(file => {
                let opt = document.createElement('option');
                opt.value = file.name;
                let sizeKB = (file.size / 1024).toFixed(1);
                opt.textContent = `${file.date || file.name} (${sizeKB} KB)`;
                list.appendChild(opt);
            });
        })
        .catch(err => {
            console.error('Error fetching logs:', err);
            list.innerHTML = '<option>Connection error</option>';
        });
    }

    function downloadSelectedLog() {
      const list = document.getElementById('logFilesList');
      const fileName = list.value;
    
      if (!fileName || fileName.includes(' ')) return; 

      console.log("Downloading log:", fileName);
      window.location.href = `/download_log?file=${fileName}`;
    }

    window.onclick = function(event) {
      if (!event.target.matches('#logBtn') && !event.target.closest('#logMenu')) {
        const menu = document.getElementById('logMenu');
        if (menu) menu.style.display = 'none';
      }
    }

let lastDataReceived = Date.now();

const CONNECTION_STATUS = {
  ACTIVE: {
    text: 'Communication session active',
    dotClass: 'status-active',
    textClass: 'text-active',
    timeout: 5000
  },
  WAITING: {
    text: 'Waiting for telemetry packet…',
    dotClass: 'status-waiting',
    textClass: 'text-waiting',
    timeout: 40000
  },
  LOST: {
    text: 'Communication lost',
    dotClass: 'status-lost',
    textClass: 'text-lost',
    timeout: Infinity
  }
};

function updateConnectionStatus() {
  const timeSinceLastData = Date.now() - lastDataReceived;
  const statusDot = document.getElementById('statusDot');
  const statusText = document.getElementById('statusText');
  
  let currentStatus;
  
  if (timeSinceLastData < CONNECTION_STATUS.ACTIVE.timeout) {
    currentStatus = CONNECTION_STATUS.ACTIVE;
  } else if (timeSinceLastData < CONNECTION_STATUS.WAITING.timeout) {
    currentStatus = CONNECTION_STATUS.WAITING;
  } else {
    currentStatus = CONNECTION_STATUS.LOST;
  }
  
  if (statusText.textContent !== currentStatus.text) {
    statusDot.classList.remove('status-active', 'status-waiting', 'status-lost');
    statusText.classList.remove('text-active', 'text-waiting', 'text-lost');
    
    statusDot.classList.add(currentStatus.dotClass);
    statusText.classList.add(currentStatus.textClass);
    statusText.textContent = currentStatus.text;
    
    console.log('[STATUS] Connection status changed:', currentStatus.text);
  }
}

setInterval(updateConnectionStatus, 1000);

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
    
  var isRequestPending = false;
  function fetchDataPeriodically() {
      setInterval(function() {
        if (isRequestPending) {
          console.log("Skipping request: previous one is still pending...");
          return; 
        }
        var xhttp = new XMLHttpRequest();
        isRequestPending = true;
        xhttp.onreadystatechange = function() {
            if (xhttp.readyState === 4) {
              isRequestPending = false;
              if (xhttp.status === 200) {
                lastDataReceived = Date.now();
                updateConnectionStatus();
                var responseData = JSON.parse(xhttp.responseText);
                let target = responseData.motor_state ? 75 : 3;

                if (wing_angle !== target) {
                  motor_direction = wing_angle < target ? 1 : -1;
                  turning();
                }
                console.log('Received JSON data:', responseData);
                if (responseData.callSign) {
                  document.getElementById("callSign").textContent = responseData.callSign;
                }
                document.getElementById("temperature").textContent = responseData.temperature.toFixed(2) + ' °C';
                let bV = responseData.battery_v;
                let bC = responseData.battery_c;
                document.getElementById("battery_v").textContent = bV.toFixed(2)+ ' V';
                let chargingElement = document.getElementById("charging_status");
                if (bV > 4.0 && bC < -5.0) {
                  chargingElement.textContent = "Battery charging";
                } else {
                  chargingElement.textContent = ""; 
                }
                let sV = responseData.solarPanels_v;
                document.getElementById("solarPanels_v").textContent = sV.toFixed(2) + ' V';
                let barText = "";
                let fullSteps = Math.floor(sV);
                for (let i = 1; i <= 7; i++) {
                  barText += (i <= fullSteps) ? "▮" : "▯";
                }
                document.getElementById("solar_bar").textContent = barText;
                document.getElementById("gasResistance").textContent = 'G.\u03A9 = ' + responseData.gas_resistance.toFixed(2) + ' KOhm';
                
                document.getElementById("roll").textContent = Math.round(responseData.roll) + '°';
                document.getElementById("pitch").textContent = Math.round(responseData.pitch) + '°';
                document.getElementById("yaw").textContent = Math.round(responseData.yaw) + '°';

                let iaqScore = responseData.iaq_score; 
                let iaqAccuracy = responseData.iaq_accuracy;
                let dataReady = responseData.data_ready;

                let color = '#808080'; 
                let iaqStatusText = 'IAQ';
                let qualityText = ''; 

                let iaqElement = document.getElementById("gas-res-text");
                let borderElement = document.getElementById("gas-res-border");
                let qualityElement = document.getElementById("data-quality-text"); 

                if (!dataReady) {
                    qualityText = 'Please wait...';
                    color = '#808080'; 
                } else if (iaqAccuracy === 0) {
                    qualityText = 'Status: Stabilizing';
                    color = '#A9A9A9';
                } else {
                    let score = Math.round(iaqScore);
                    iaqStatusText = score + ' IAQ';

                    if (iaqAccuracy === 1) {
                        qualityText = 'Data quality: ⚫️⚪️⚪️ LOW';
                        color = '#A9A9A9'; 
                    } else if (iaqAccuracy === 2) {
                        qualityText = 'Data quality: ⚫️⚫️⚪️ MEDIUM';
                    } else if (iaqAccuracy === 3) {
                        qualityText = 'Data quality: ⚫️⚫️⚫️ HIGH';
                    }
                        
                    if (iaqAccuracy >= 2) {
        
                      if (score <= 50) {
                        color = '#00FF00'; // Excellent
                    } else if (score <= 100) {
                        color = '#9ACD32'; // Good
                    } else if (score <= 150) {
                        color = '#FFA500'; // Lightly Polluted
                    } else if (score <= 200) {
                        color = '#FF4500'; // Moderately Polluted
                    } else {
                        color = '#8B0000'; // Heavily Polluted
                    }
                }
                }

                iaqElement.textContent = iaqStatusText;
                qualityElement.textContent = qualityText;
                borderElement.style.border = "15px solid " + color;

                let photoBtn = document.getElementById("photoBtn");
                if (photoBtn) {
                  if (responseData.camera_ready) {
                    photoBtn.disabled = false;
                    photoBtn.style.opacity = "1"; 
                    photoBtn.style.cursor = "pointer"; 
                  } else {
                    photoBtn.disabled = true;
                    photoBtn.style.opacity = "0.5";
                    photoBtn.style.cursor = "not-allowed"; 
                    photoBtn.title = "Camera not found!";
                  }
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
            if(year === 0){
              document.getElementById("date_time_mysat").textContent = 'Date and time: ▲ RTC module not found';
            } else{
              let date_time_string = new Date(year, month - 1, day, hour, minute, second);
              document.getElementById("date_time_mysat").textContent = 'Date and time: ' + date_time_string.toLocaleString("uk-UA");
              console.log(date_time_string);
            }
              } else {
                console.error('Error fetching data');
              }
            }
        };
        xhttp.timeout = 4000; 
        xhttp.ontimeout = function () { 
          console.log("Timeout!"); 
          isRequestPending = false; 
        };
        xhttp.open('GET', '/get_data', true);
        xhttp.send();
      }, 2000);   
  }
    

fetchDataPeriodically();
fetchPhotoList();

    </script>
</body>
</html>
)###";


WebServer server(80);

String json_string = "";

const size_t bufferSize = JSON_OBJECT_SIZE(35);

DynamicJsonDocument json_sensors(bufferSize);

String* generateSensorsDataJson(pointer_of_sensors* data_, bool motor_state) {
  unsigned long t_start = millis();
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
    json_sensors["iaq_score"] = data_->bme_->iaq;
    json_sensors["iaq_accuracy"] = data_->bme_->iaq_accuracy;
    json_sensors["data_ready"] = data_->bme_->data_available;
  } else {
    json_sensors["temperature"] = 0;
    json_sensors["humidity"] = 0;
    json_sensors["gas_resistance"] = 0;
    json_sensors["pressure"] = 0;
    json_sensors["iaq_score"] = 0;
    json_sensors["iaq_accuracy"] = 0;
    json_sensors["data_ready"] = false;
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
  if (init_status.mpu_ && calibration.valid) {
    json_sensors["yaw"] = data_->mpu_->yaw   - offset_yaw;
    json_sensors["pitch"] = data_->mpu_->pitch - offset_pitch;
    json_sensors["roll"] = data_->mpu_->roll  - offset_roll;
  } else {
    json_sensors["yaw"] = 0;
    json_sensors["pitch"] = 0;
    json_sensors["roll"] = 0;
  }
  if(init_status.ina_){
    json_sensors["battery_v"] = data_->ina_->batteryVoltage;
    json_sensors["battery_c"] = data_->ina_->batteryCurrent;
    json_sensors["solarPanels_v"] = data_->ina_->SolarPanelVoltage;
  }else{
    json_sensors["battery_v"] = 0;
    json_sensors["battery_c"] = 0;
    json_sensors["solarPanels_v"] = 0;
  }
  json_sensors["motor_state"] = motor_state;
  json_sensors["callSign"] = callSign;
  json_sensors["camera_ready"] = init_status.camera_;
  serializeJson(json_sensors, json_string);

  if(debug_mode_active){
    unsigned long dt = t_start - millis();
    if (dt > 1000000) dt = 0;
    logDebug("[SERVER]JSON Generated: " + String(json_string.length()) + " bytes in " + String(dt) + " ms");
    logDebug("[SERVER] JSON: " + json_string);
  }
  return &json_string;
}

void handleRoot() {
  //Serial.println(json_string);
  logDebug("[WEB] Request: / (Root)");
  server.send(200, "text/html", htmlContent);
  logDebug("[WEB] Sent index.html");
}

void handleGetData() {
  if (debug_mode_active) {
    logDebug("[WEB] Request: /get_data | Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
  }
  pointer_of_sensors* data = get_sensors_data();
  generateSensorsDataJson(data, stateMotor);
  server.send(200, "text/plain", json_string);
}

void handleGetPhotoList() {
  logDebug("[WEB] Request: /get_photo_list");
  String json = getPhotoListJson();
  server.send(200, "application/json", json);
}

void handleGetPhotoById() {
  if (!server.hasArg("id")) {
    server.send(400, "text/plain", "Missing ID parameter");
    return;
  }

  int photoId = server.arg("id").toInt();
  logDebug("[WEB] Request: /get_photo_by_id?id=" + String(photoId));

  PhotoRecord* record = getPhotoById(photoId);
  if (!record) {
    server.send(404, "text/plain", "Photo not found");
    return;
  }

  File photoFile = LittleFS.open(record->filename.c_str(), "r");
  if (!photoFile) {
    server.send(500, "text/plain", "Failed to open photo file");
    return;
  }

  size_t fileSize = photoFile.size();
  uint8_t* buffer = (uint8_t*)malloc(fileSize);
  if (!buffer) {
    photoFile.close();
    server.send(500, "text/plain", "Memory allocation failed");
    return;
  }

  photoFile.read(buffer, fileSize);
  photoFile.close();

  String base64String = base64::encode(buffer, fileSize);
  free(buffer);

  DynamicJsonDocument doc(base64String.length() + 256);
  doc["id"] = record->id;
  doc["timestamp"] = record->timestamp;
  doc["image"] = base64String;

  String response;
  serializeJson(doc, response);

  server.send(200, "application/json", response);
  logDebug("[WEB] Sent photo #" + String(photoId));
}

void handleGetPhoto() {
  logDebug("[PHOTO] --- Request: /get_photo ---");
  if (!init_status.camera_) {
    logDebug("[PHOTO] Error: Camera not found!");
    server.send(503, "text/plain", "Camera not available"); // 503 Service Unavailable
    return;
  }
  unsigned long t_total_start = millis();

  logDebug("[PHOTO] Capture started");
  unsigned long t_cap = millis();
  camera_fb_t* old_fb = esp_camera_fb_get();
  if (old_fb) {
    esp_camera_fb_return(old_fb);
    logDebug("[PHOTO] Initial frame discarded.");
  }

  camera_fb_t* fb = esp_camera_fb_get();

  while (!fb) {
    fb = esp_camera_fb_get();
    if (fb) {
      break;
    }
  }
  if (!fb) {
    logDebug("[PHOTO] Error: Capture Failed");
    server.send(500, "text/plain", "Camera capture failed");
    return;
  }
  logDebug("[PHOTO] Captured: " + String(fb->len) + " bytes in " + String(millis() - t_cap) + " ms");

  char timestamp[25];

  if(init_status.rtc_){
    rtc_struct* current_time = get_rtc();
     snprintf(timestamp, sizeof(timestamp), "%04d-%02d-%02dT%02d:%02d:%02d",
           current_time->year_,
           current_time->month_,
           current_time->day_,
           current_time->hour_,
           current_time->minute_,
           current_time->second_);
  }else{
    strcpy(timestamp, "unknown");
  }

  if (savePhoto(fb, timestamp)) {
    DynamicJsonDocument doc(fb->len * 1.4 + 256); 
    
    String base64String = base64::encode(fb->buf, fb->len);
    
    doc["id"] = globalPhotoCounter; 
    doc["timestamp"] = timestamp;
    doc["image"] = base64String;

    String response;
    serializeJson(doc, response);
    
    server.send(200, "application/json", response);
    logDebug("[PHOTO] Successfully saved and sent photo #" + String(globalPhotoCounter));
  } else {
    server.send(500, "text/plain", "Failed to save photo");
    logDebug("[PHOTO] Failed to save photo");
  }

  esp_camera_fb_return(fb);
  logDebug("[PHOTO] --- TOTAL TIME: " + String(millis() - t_total_start) + " ms ---");
}

void handleGetLogList() {
    logDebug("[WEB] Request: /get_log_list");
    DynamicJsonDocument doc(4096); 
    JsonArray files = doc.createNestedArray("files");

    File root = LittleFS.open("/");
    File file = root.openNextFile();

    while (file) {
        String filename = String(file.name());
        if (filename.startsWith("/")) filename = filename.substring(1);

        if (filename.startsWith("log_") && filename.endsWith(".csv")) {
            JsonObject fileObj = files.createNestedObject();
            fileObj["name"] = filename;
            fileObj["size"] = file.size();

            if (filename.length() >= 20) {
                String d = filename.substring(4, 12);  
                String t = filename.substring(13, 19); 
                
                fileObj["date"] = d.substring(0, 4) + "-" + d.substring(4, 6) + "-" + d.substring(6, 8) + 
                                  " " + t.substring(0, 2) + ":" + t.substring(2, 4);
            } else {
                fileObj["date"] = "Unknown Date";
            }
        }
        file = root.openNextFile();
    }
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleDownloadLog() {
  if (!server.hasArg("file")) {
    server.send(400, "text/plain", "Missing file parameter");
    return;
  }
  
  String filename = "/" + server.arg("file");
  logDebug("[WEB] Request: /download_log?file=" + filename);
  
  if (!LittleFS.exists(filename.c_str())) {
    server.send(404, "text/plain", "File not found");
    return;
  }
  
  File file = LittleFS.open(filename.c_str(), "r");
  if (!file) {
    server.send(500, "text/plain", "Failed to open file");
    return;
  }
  
  server.sendHeader("Content-Disposition", "attachment; filename=\"" + server.arg("file") + "\"");
  server.streamFile(file, "text/csv");
  file.close();
  
  logDebug("[WEB] File downloaded: " + filename);
}

bool stateLight = false;

void light_on() {
  logDebug("[WEB] Request: /light_on");
  stateLight = !stateLight;
  control_light(stateLight);
  server.send(200, "text/plain", "OK");
  logDebug("[WEB] LED Toggled");
}

void motor_on() {
  logDebug("[WEB] Request: /motor_on");
  setStateMotor(!stateMotor);
  server.send(200, "text/plain", "OK");
  logDebug("[WEB] Motor Toggled");
}

void initServer() {
  initPhotoStorage();
  server.on("/", handleRoot);
  server.on("/get_data", handleGetData);
  server.on("/light_on", HTTP_GET, light_on);
  server.on("/motor_on", HTTP_GET, motor_on);
  server.on("/get_photo", HTTP_GET, handleGetPhoto);
  server.on("/get_photo_list", HTTP_GET, handleGetPhotoList);
  server.on("/get_photo_by_id", HTTP_GET, handleGetPhotoById);
  server.on("/get_log_list", HTTP_GET, handleGetLogList);
  server.on("/download_log", HTTP_GET, handleDownloadLog);

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
  LOG_INFO("[SERVER] HTTP server started");
}