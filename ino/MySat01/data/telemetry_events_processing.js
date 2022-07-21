function renew(){
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
 source.addEventListener('name_probe', function(e) {
  console.log("name_probe", e.data);
  document.getElementById("name_probe").innerHTML = e.data;
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
 source.addEventListener('num_position', function(e) {
  console.log("num_position", e.data);
  document.getElementById("num_position").innerHTML = e.data;
  }, false);
}

function capturePhoto() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/capture", true);
    xhr.send();
 }
 function switchLED() {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', "/switch", true);
    xhr.send();
 }
 
