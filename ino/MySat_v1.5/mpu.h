#include <MPU9250_asukiaaa.h>

MPU9250_asukiaaa mpu_sensor(0x69);


struct mpu{
  float aX;
  float aY;
  float aZ;
  float gX;
  float gY;
  float gZ;
  float mX;
  float mY;
  float mZ;
  } mpu_data;

bool initMPU(){
  uint8_t sensorId;
  mpu_sensor.setWire(&Wire);
  mpu_sensor.beginAccel();
  mpu_sensor.beginGyro();
  mpu_sensor.beginMag();
  return true;
}

mpu * get_mpu_data(){
   mpu_sensor.accelUpdate();
   mpu_sensor.gyroUpdate();
   mpu_sensor.magUpdate();
   mpu_data.aX = mpu_sensor.accelX();
   mpu_data.aY = mpu_sensor.accelY();
   mpu_data.aZ = mpu_sensor.accelZ();

   mpu_data.gX = mpu_sensor.gyroX();
   mpu_data.gY = mpu_sensor.gyroY();
   mpu_data.gZ = mpu_sensor.gyroZ();

   mpu_data.mX = mpu_sensor.magX();
   mpu_data.mY = mpu_sensor.magY();
   mpu_data.mZ = mpu_sensor.magZ();
   return &mpu_data;
}

void print_data(mpu * data_){
    /*
    uint8_t sensorId = 0x69;
    if (mySensor.readId(&sensorId) == 0) 
        Serial.println("MPU " + String(sensorId));
    else 
        Serial.println("Cannot read sensorId");
    */
    Serial.print("aX = ");
    Serial.print(data_->aX);
    Serial.print(" | ");
    Serial.print("aY = ");
    Serial.print(data_->aY);
    Serial.print(" | ");
    Serial.print("aZ = ");
    Serial.println(data_->aZ);

    Serial.println("------------------------------------");
    
    Serial.print("gX = ");
    Serial.print(data_->gX);
    Serial.print(" | ");
    Serial.print("gY = ");
    Serial.print(data_->gY);
    Serial.print(" | ");
    Serial.print("gZ = ");
    Serial.println(data_->gZ);

    Serial.println("------------------------------------");
    
    Serial.print("mX = ");
    Serial.print(data_->mX);
    Serial.print(" | ");
    Serial.print("mY = ");
    Serial.print(data_->mY);
    Serial.print(" | ");
    Serial.print("mZ = ");
    Serial.println(data_->mZ);
}
