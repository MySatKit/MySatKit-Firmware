//for DS3231 - Real Time Clock (RTC) on MySat

//used to control the actual time during power off of the main microcontroller

#include <RtcDS3231.h>
#include <Wire.h>
RtcDS3231<TwoWire> Rtc(Wire);

struct rtc_struct{
  int year_;
  int month_;
  int day_;
  int hour_;
  int minute_;
  int second_;  
} rtc_data;

bool initRTC(){
  Rtc.Begin();
  return Rtc.GetIsRunning();
}

void setRTC(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second){
  RtcDateTime dt(year, month, day, hour, minute, second);
  Rtc.SetDateTime(dt);
}

void readUARTTime(){
  Serial.println("Please set time. Enter data only in numbers!");

  uint16_t year;
  uint8_t month, day, hour, minute, second;

  Serial.println("Enter year:");
  while (!Serial.available()) {}
  year = Serial.readStringUntil('\n').toInt();

  Serial.println("Enter month:");
  while (!Serial.available()) {}
  month = Serial.readStringUntil('\n').toInt();

  Serial.println("Enter day:");
  while (!Serial.available()) {}
  day = Serial.readStringUntil('\n').toInt();

  Serial.println("Enter hour:");
  while (!Serial.available()) {}
  hour = Serial.readStringUntil('\n').toInt();

  Serial.println("Enter minute:");
  while (!Serial.available()) {}
  minute = Serial.readStringUntil('\n').toInt();

  Serial.println("Enter second:");
  while (!Serial.available()) {}
  second = Serial.readStringUntil('\n').toInt();

  if (year < 2000 || year > 2100 || month < 1 || month > 12 || day < 1 || day > 31 ||
      hour > 23 || minute > 59 || second > 59) {
    Serial.println("Invalid input! Please try again.");
    return;
  }

  setRTC(year, month, day, hour, minute, second);
  Serial.println("Time is now set!");
}

void setTime(){
  RtcDateTime now = Rtc.GetDateTime();

  if (!now.IsValid()) {
    Serial.println("RTC time is invalid. Please set time.");
    readUARTTime();
  } else {
    Serial.println("If you want to change the time, use the command: TIME ");
  }
}

rtc_struct * get_rtc(){
  RtcDateTime dt = Rtc.GetDateTime();
  rtc_data.year_ = dt.Year();
  rtc_data.month_ = dt.Month();
  rtc_data.day_ = dt.Day();
  rtc_data.hour_ = dt.Hour();
  rtc_data.minute_ = dt.Minute();
  rtc_data.second_ = dt.Second();
  return &rtc_data;
}

void print_data(rtc_struct * data_){
    Serial.print("Time: ");
    Serial.print(data_->year_);
    Serial.print(".");
    if(data_->month_ < 10){
      Serial.print(0);
      Serial.print(data_->month_);
    } else{
      Serial.print(data_->month_);  
    }
    Serial.print(".");
    if(data_->day_ < 10){
      Serial.print(0);
      Serial.print(data_->day_);
    } else{
      Serial.print(data_->day_);  
    }
    Serial.print("  ");
    if(data_->hour_ < 10){
      Serial.print(0);
      Serial.print(data_->hour_);
    } else{
      Serial.print(data_->hour_);  
    }
    Serial.print(":");
    if(data_->minute_ < 10){
      Serial.print(0);
      Serial.print(data_->minute_);
    } else{
      Serial.print(data_->minute_);  
    }
    Serial.print(":");
    if(data_->second_ < 10){
      Serial.print(0);
      Serial.println(data_->second_);
    } else{
      Serial.println(data_->second_);  
    }
}
