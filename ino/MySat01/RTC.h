#pragma once

#include <Wire.h> 
#include <RtcDS3231.h>
#include "I2C.h"

static RtcDS3231<TwoWire> Rtc(getI2C());

int initRTC(void)
{
    Rtc.Begin();
}

RtcDateTime rtcGetTime()
{
    return Rtc.GetDateTime();
}
void rtcSetTime(int setYear, int setMonth, int setDay, int setHour, int setMinute, int setSecond){
  Rtc.SetDateTime(RtcDateTime(setYear, setMonth, setDay, setHour, setMinute, setSecond));
  
  }
RtcTemperature rtcGetTemperature()
{
    return Rtc.GetTemperature();
}
