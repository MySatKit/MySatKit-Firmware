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
