//ensures the operation of the MySat main console (web-interface)

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "sensors_data.h"
#include "control.h"
#include "base64.h"

extern String ssid;
extern String password;

const char* htmlContent = R"###(
<!DOCTYPE html>
<html>
<head>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.1/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-4bw+/aepP/YC94hEpVNVgiZdgIC5+VKNBQNGCHeKRQN+PtmoHDEXuppvnDJzQIu9" crossorigin="anonymous">
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
  background:#000 url(https://i.postimg.cc/hhFD7Nkf/apps-47636-13819498107583324-fb06b53a-4974-4acb-85e6-f6f363f85ab6-edec48d0-ec35-4a1f-8bf5-c455987b4a.png) repeat top center;
  z-index: 0;
}

.twinkling {
  background-image: url(https://i.postimg.cc/CLpNr4PF/twinkling.png);
  animation: 3s twinkle infinite;
  animation-timing-function: linear;
}

@keyframes twinkle {
  from {backrgound-position: 0}
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

.sunlightTrackerCircle{
   width:80%;
   padding-top: 80%;
   border:2px solid white;
   border-radius:100%;
   position: relative;
   margin-left:10%;
}

.sunImg{
  background-image: url("https://i.ibb.co/gz12c7t/Sunlight.png");
  background-size: cover;
  width: 40px; 
  height: 40px; 
  position: absolute; 
  top: 0%; 
  left: 120%;
  border: 0;
  transform: translate(-370%, -50%);
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
			background-image:url(https://i.ibb.co/wWJpx5h/front.png);
		}
		
		.left {
			background: yellow;
			transform: rotateY(-90deg) translateZ(50px);
			background-image:url(https://i.ibb.co/jkf6TK6/side.png);
		}
		
		.right {
			background: orange;
			transform: rotateY(90deg) translateZ(50px);
			background-image:url(https://i.ibb.co/jkf6TK6/side.png);
		}
		
		.top {
			background: green;
			transform: rotateX(90deg) translateZ(50px);
			background-image:url(https://i.ibb.co/4ZZ0FJS/plate.png);
		}
		
		.bottom {
			background: black;
			transform: rotateX(-90deg) translateZ(50px);
			background-image:url(https://i.ibb.co/4ZZ0FJS/plate.png);
		}
		
		.back {
			background: brown;
			transform: rotateY(180deg) translateZ(50px);
			background-image:url(https://i.ibb.co/FDfxmRm/shield.png);
		}
		
		.rightwing {
			background: LightGreen;
			transform-origin: 50% 0%;
			transform: rotateY(90deg) translateZ(60px) rotateX(3deg);
			background-image:url(https://i.ibb.co/MsrKsLG/solar.png);
			width: 80px;
		}
		.leftwing {
			background: LightGreen;
			transform-origin: 50% 0%;
			transform: rotateY(-90deg) translateZ(40px) rotateX(3deg);
			background-image:url(https://i.ibb.co/MsrKsLG/solar.png);
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
        <img src="https://i.ibb.co/5RrGwm5/photo.png" alt="" id = "photoFromESP">
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
              <h3 id = "text_data">Accelerometer:</h3>
              <h4 id = "ax">aX: 0</h3>
              <h4 id = "ay">aY: 0</h3>
              <h4 id = "az">aZ: 0</h3>
            </div>
            <div class = "col-lg-4 text-data m-2">
              <h3 id = "text_data">Gyroscope:</h3>
              <h4 id = "gx">gX: 0</h3>
              <h4 id = "gy">gY: 0</h3>
              <h4 id = "gz">gZ: 0</h3>
            </div>
            <div class = "col-lg-3 text-data m-2">
              <h3 id = "text_data">Compass:</h3>
              <h4 id = "mx"></h3>
              <h5 id = "my">( magnetometer </h3>
              <h5 id = "mz">is off )</h3>
            </div>
            
          </div>
          <div class = "row">
            <div class = "col-lg-4 text-data m-2">
              <div class="text-data-sunLight">
                <h3>Sunlight trackers</h3>
                <div class="sunlightTrackerCircle">
                  <div class="sunImg" id = "sun_img"></div>
                </div>
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
			
			
			if (wing_angle==75) { 
				motor_direction = -1;
			}
			else {
				motor_direction = 1;
			}
			turning();	
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
		
		
  function fetchDataPeriodically() {
      setInterval(function() {
        var xhttp = new XMLHttpRequest();

        xhttp.onreadystatechange = function() {
            if (xhttp.readyState === 4) {
                if (xhttp.status === 200) {
                    var responseData = JSON.parse(xhttp.responseText);
                    console.log('Received JSON data:', responseData);
                    document.getElementById("temperature").textContent = responseData.temperature.toFixed(2) + ' C';
                    document.getElementById("pressure").textContent = responseData.pressure.toFixed(2)+ ' Pa';
                    document.getElementById("humidity").textContent = responseData.humidity.toFixed(2) + ' %';
                    
                    document.getElementById("ax").textContent = "aX: " + Math.round((responseData.ax)*100)/100;
                    document.getElementById("ay").textContent = "aY: " + Math.round((responseData.ay)*100)/100;
                    document.getElementById("az").textContent = "aZ: " + Math.round((responseData.az)*100)/100;
          
                    document.getElementById("gx").textContent = "gX: " + Math.round((responseData.gx)*100)/100;
                    document.getElementById("gy").textContent = "gY: " + Math.round((responseData.gy)*100)/100;
                    document.getElementById("gz").textContent = "gZ: " + Math.round((responseData.gz)*100)/100;

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
      
                    let ph1 = responseData.ph1;
                    let ph2 = responseData.ph2;
                    let ph3 = responseData.ph3;
                    let ph4 = responseData.ph4;
                    
                    let phs = [ph1, ph2, ph3, ph4];
                    let min_ph = phs[0]
                     for(let i = 0;i < phs.length; i++){
                        if(min_ph > phs[i]){
                          min_ph = phs[i]; 
                         } 
                     }
                    let direction_ = phs.indexOf(min_ph);
        if(direction_ == 0){
      document.getElementById("sun_img").style.left = "72%";
      document.getElementById("sun_img").style.top = "50%";
        }
        if(direction_ == 3){
      document.getElementById("sun_img").style.left = "120%";
      document.getElementById("sun_img").style.top = "0%";
        }
        if(direction_ == 2){
      document.getElementById("sun_img").style.left = "172%";
      document.getElementById("sun_img").style.top = "50%";
        }
        if(direction_ == 3){
      document.getElementById("sun_img").style.left = "120%";
      document.getElementById("sun_img").style.top = "100%";
        }
                
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
xhttp.send();


    </script>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.1/dist/js/bootstrap.bundle.min.js" integrity="sha384-HwwvtgBNo3bZJJLYd8oVXjrBZt8cqVSpeBNS5n7C8IVInixGAoxmnlMuBnhbgrkm" crossorigin="anonymous"></script>
</body>
</html>
)###";


WebServer server(80);

bool stateMotor = false;

void handleRoot() {
  Serial.println(json_string);
  server.send(200, "text/html", htmlContent);
}

void handleGetData() {
  server.send(200, "text/plain", json_string);
}

void handleGetPhoto() {
  Serial.println("Create Photo");
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
  server.send(200, "text/plain", base64String);
  Serial.print("Photo sended");
  esp_camera_fb_return(fb);
}

bool stateLight = false;

void light_on() {
  stateLight = !stateLight;
  control_light(stateLight);
}

void motor_on() {
  stateMotor = !stateMotor;
  control_motor(stateMotor);
}

void initServer() {
  server.on("/", handleRoot);
  server.on("/get_data", handleGetData);
  server.on("/light_on", HTTP_GET, light_on);
  server.on("/motor_on", HTTP_GET, motor_on);
  server.on("/get_photo", HTTP_GET, handleGetPhoto);
  server.begin();
  Serial.println("HTTP server started");
}
