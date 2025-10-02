//for MPU - MySat accelerometer, gyroscope

//used to determine the position of a satellite relative to the Earth's gravitational field

#pragma once
#include <SPIFFS.h>
#define MPU_ADDRESS 0x69

#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H 0x3B
#define GYRO_XOUT_H 0x43
#define TEMP_OUT_H 0x41

struct mpu {
  float yaw, pitch, roll;
  bool dataReady;
} mpu_data;

struct CalibrationData {
  int16_t gyro_x, gyro_y, gyro_z;  // offsets of gyro
  bool valid;
} calibration = { 0, 0, 0, false };

// data for sensor fusion
float angle_x = 0, angle_y = 0, angle_z = 0;
unsigned long lastMicros = 0;
bool mpuReady = false;

float offset_yaw, offset_pitch, offset_roll; //for calibration

void writeRegister(uint8_t reg, uint8_t value);
void loadCalibration();
void saveCalibration();

bool initMPU() {
  Wire.beginTransmission(MPU_ADDRESS);
  if (Wire.endTransmission() != 0) {
    return false;
  }

  writeRegister(PWR_MGMT_1, 0x80);  // reset
  delay(100);
  writeRegister(PWR_MGMT_1, 0x00);
  delay(10);


  writeRegister(0x1A, 3);  
  writeRegister(0x19, 4);  
  writeRegister(0x1B, 0x00);
  writeRegister(0x1C, 0x00);

  loadCalibration();

  mpuReady = true;
  lastMicros = micros();
  return true;
}

uint8_t readRegister(uint8_t reg) {
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDRESS, (uint8_t)1);
  if (Wire.available()) {
    return Wire.read();
  }
  return 0;
}

void writeRegister(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
  delay(1);
}

bool readRegistersBurst(uint8_t reg, uint8_t *buf, uint8_t len) {
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  uint8_t got = Wire.requestFrom(MPU_ADDRESS, len);
  if (got != len) return false;
  for (uint8_t i = 0; i < len; i++) buf[i] = Wire.read();
  return true;
}

int16_t raw16(const uint8_t *buf, uint8_t idx) {
  return (int16_t)((buf[idx] << 8) | buf[idx+1]);
}

mpu * get_mpu_data() {
  const int SAMPLE_RATE = 200;      
  const int SAMPLES_PER_CALL = 20;   
  const float ALPHA = 0.95f;        

  static bool first = true;
  if (first) {
    lastMicros = micros();
    first = false;
  }

  for (int s = 0; s < SAMPLES_PER_CALL; s++) {
    uint8_t buf[14];
    if (!readRegistersBurst(ACCEL_XOUT_H, buf, 14)) break;

    int16_t ax = raw16(buf, 0);
    int16_t ay = raw16(buf, 2);
    int16_t az = raw16(buf, 4);
    int16_t gx = raw16(buf, 8);
    int16_t gy = raw16(buf, 10);
    int16_t gz = raw16(buf, 12);

    if (calibration.valid) {
      gx -= calibration.gyro_x;
      gy -= calibration.gyro_y;
      gz -= calibration.gyro_z;
    }

    float accel_x = ax / 16384.0f;
    float accel_y = ay / 16384.0f;
    float accel_z = az / 16384.0f;

    float gyro_x = gx / 131.0f;
    float gyro_y = gy / 131.0f;
    float gyro_z = gz / 131.0f;

    unsigned long now = micros();
    float dt = (now - lastMicros) / 1000000.0f;
    lastMicros = now;
    if (dt <= 0 || dt > 0.1f) dt = 1.0f / SAMPLE_RATE;

    angle_x += gyro_x * dt;
    angle_y += gyro_y * dt;
    angle_z += gyro_z * dt;

    float accel_pitch = atan2(-accel_x, sqrt(accel_y * accel_y + accel_z * accel_z)) * 180.0f / PI;
    float accel_roll  = atan2(accel_y, accel_z) * 180.0f / PI;

    angle_x = ALPHA * angle_x + (1.0f - ALPHA) * accel_roll;
    angle_y = ALPHA * angle_y + (1.0f - ALPHA) * accel_pitch;

    if (angle_z > 180) angle_z -= 360;
    if (angle_z < -180) angle_z += 360;

    long targetDelay = 1000000 / SAMPLE_RATE;
    unsigned long used = micros() - now;
    if (used < (unsigned long)targetDelay) {
      delayMicroseconds(targetDelay - used);
    }
  }

  mpu_data.roll = angle_x;
  mpu_data.pitch = angle_y;
  mpu_data.yaw = angle_z;
  mpu_data.dataReady = true;
  return &mpu_data;
}

void calibrateMPU() {
  Serial.println("Calibration...");
  Serial.println("Put the device on a surface and do not move for 5 seconds!");
  delay(2000);

  long sum_gx = 0, sum_gy = 0, sum_gz = 0;
  int samples = 500;

  for (int i = 0; i < samples; i++) {
    uint8_t buf[14];
    if (!readRegistersBurst(ACCEL_XOUT_H, buf, 14)) continue;
    int16_t gx = raw16(buf, 8);
    int16_t gy = raw16(buf, 10);
    int16_t gz = raw16(buf, 12);

    sum_gx += gx;
    sum_gy += gy;
    sum_gz += gz;

    if (i % 50 == 0) {
      Serial.printf(" Progress: %d%%\n", (i * 100) / samples);
    }
    delay(10);
  }

  calibration.gyro_x = sum_gx / samples;
  calibration.gyro_y = sum_gy / samples;
  calibration.gyro_z = sum_gz / samples;
  calibration.valid = true;

  saveCalibration();

  mpu* data = get_mpu_data();
  offset_roll = data->roll;
  offset_pitch = data->pitch;
  offset_yaw = data->yaw;

  Serial.println("Calibration is complete");
}

void saveCalibration() {
  if (!SPIFFS.begin(true)) return;

  File file = SPIFFS.open("/cal.dat", "w");
  if (file) {
    uint32_t magic = 0x6500ABCD;
    file.write((uint8_t*)&magic, 4);
    file.write((uint8_t*)&calibration, sizeof(calibration));
    file.close();
    Serial.println("Calibration is saved");
  }
}

void loadCalibration() {
  if (!SPIFFS.begin(true)) return;

  File file = SPIFFS.open("/cal.dat", "r");
  if (file && file.size() >= 4 + sizeof(calibration)) {
    uint32_t magic;
    file.read((uint8_t*)&magic, 4);
    if (magic == 0x6500ABCD) {
      file.read((uint8_t*)&calibration, sizeof(calibration));
      Serial.println("Calibration is loaded");
    }
    file.close();
  }
}
